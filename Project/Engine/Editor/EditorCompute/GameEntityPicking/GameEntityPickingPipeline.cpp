#include "GameEntityPickingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Graphics/Framework/RenderInfo.h"

/// editor
#include "Engine/Editor/Math/ImGuiSelection.h"

using namespace Editor;
using namespace ONEngine;


GameEntityPickingPipeline::GameEntityPickingPipeline() = default;
GameEntityPickingPipeline::~GameEntityPickingPipeline() = default;

void GameEntityPickingPipeline::Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) {

	pDxm_ = _dxm;

	{	/// shader/pipeline
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/GameEntityPicking.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // CBV_PARAMS

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_PICKING
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_FLAGS_TEXTURE

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // ROOT_PARAM_CBV_PARAMS
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // ROOT_PARAM_UAV_PICKING

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0); // s0

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

	{	/// buffer
		cbufPickingParams_.Create(_dxm->GetDxDevice());
		sbufPicking_.CreateUAV(1, _dxm->GetDxDevice(), _dxm->GetDxCommand(), _dxm->GetDxSRVHeap());
	}

}

void GameEntityPickingPipeline::Execute(
	ONEngine::EntityComponentSystem* _ecs,
	ONEngine::DxCommand* _dxCommand,
	ONEngine::AssetCollection* _assetCollection) {

	Vector2 mousePosNorm = Input::GetImGuiImageMousePosNormalized("Scene");
	mousePosNorm /= Vector2::HD;

	/// テクスチャ外は処理できない
	if(mousePosNorm.x < 0.0f || mousePosNorm.x > 1.0f ||
	   mousePosNorm.y < 0.0f || mousePosNorm.y > 1.0f) {
		return;
	}

	cbufPickingParams_.SetMappedData({ mousePosNorm });


	/// ---------------------------------------------------
	/// パイプラインの設定・起動を行う
	/// ---------------------------------------------------

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto cmdList = _dxCommand->GetCommandList();

	cbufPickingParams_.BindForComputeCommandList(cmdList, CBV_PICKING_PARAMS);
	sbufPicking_.UAVBindForComputeCommandList(cmdList, UAV_PICKING);

	const ONEngine::Texture* flagsTexture = _assetCollection->GetTexture(
		RenderInfo::kRenderTargetDir +
		RenderInfo::kRenderTargetNames[static_cast<int>(RenderInfo::RenderTexture::Debug)] +
		RenderInfo::kRenderTargetType[static_cast<int>(RenderInfo::RenderTextureType::Flags)]
	);

	cmdList->SetComputeRootDescriptorTable(
		SRV_FLAGS_TEXTURE, flagsTexture->GetSRVGPUHandle()
	);

	cmdList->Dispatch(1, 1, 1);

	Picking out;
	ReadbackPickingData(_dxCommand, out);

	if(Input::TriggerMouse(Mouse::Left)) {
		if(ECSGroup* current = _ecs->GetCurrentGroup()) {
			if(GameEntity* entity = current->GetEntity(out.entityId)) {
				ImGuiSelection::SetSelectedObject(entity->GetGuid(), SelectionType::Entity);
			}
		}

	}

}

void Editor::GameEntityPickingPipeline::ReadbackPickingData(ONEngine::DxCommand* _dxCommand, Picking& _outPickingData) {

	// UAVバリアを設定してGPU書き込み完了を待機
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = sbufPicking_.GetResource().Get();
	_dxCommand->GetCommandList()->ResourceBarrier(1, &barrier);

	// コマンドリストを実行してGPU処理を完了
	_dxCommand->CommandExecute();
	_dxCommand->WaitForGpuComplete();

	// readbackバッファを作成
	ID3D12Resource* readbackBuffer = nullptr;
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_READBACK;

	D3D12_RESOURCE_DESC resourceDesc = sbufPicking_.GetResource().Get()->GetDesc();
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	HRESULT hr = pDxm_->GetDxDevice()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&readbackBuffer)
	);

	if(FAILED(hr)) {
		return;
	}

	// コピー用のコマンドリストをリセット
	_dxCommand->CommandReset();

	pDxm_->HeapBindToCommandList();

	// UAVバッファからreadbackバッファへコピー
	_dxCommand->GetCommandList()->CopyResource(readbackBuffer, sbufPicking_.GetResource().Get());

	// コマンド実行
	_dxCommand->CommandExecute();
	_dxCommand->WaitForGpuComplete();

	// データをマップして読み取り
	void* mappedData = nullptr;
	D3D12_RANGE readRange = { 0, sizeof(Picking) };
	hr = readbackBuffer->Map(0, &readRange, &mappedData);

	if(SUCCEEDED(hr)) {
		memcpy(&_outPickingData, mappedData, sizeof(Picking));
		readbackBuffer->Unmap(0, nullptr);
	}

	// readbackバッファを解放
	readbackBuffer->Release();

	// コマンドリストをリセットして次の処理に備える
	_dxCommand->CommandReset();
	pDxm_->HeapBindToCommandList();

}
