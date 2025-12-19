#include "GameEntityPickingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Framework/RenderInfo.h"


using namespace Editor;
using namespace ONEngine;


GameEntityPickingPipeline::GameEntityPickingPipeline() = default;
GameEntityPickingPipeline::~GameEntityPickingPipeline() = default;

void GameEntityPickingPipeline::Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) {

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

	const Vector2& mousePosNorm = Input::GetImGuiImageMousePosNormalized("Scene");

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
		RenderInfo::kRenderTargetNames[static_cast<int>(RenderInfo::RenderTexture::Scene)] +
		RenderInfo::kRenderTargetType[static_cast<int>(RenderInfo::RenderTextureType::Flags)]
	);

	cmdList->SetComputeRootShaderResourceView(
		SRV_FLAGS_TEXTURE, flagsTexture->GetSRVGPUHandle().ptr
	);

	cmdList->Dispatch(1, 1, 1);
}