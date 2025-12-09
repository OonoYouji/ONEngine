#include "GrassRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/EntityComponentSystem/ECSGroup.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"
#include "Engine/Asset/Collection/AssetCollection.h"


GrassRenderingPipeline::GrassRenderingPipeline(AssetCollection* _assetCollection) : pAssetCollection_(_assetCollection) {};
GrassRenderingPipeline::~GrassRenderingPipeline() = default;

void GrassRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	pDxManager_ = _dxm;

	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);

		/// mesh shader
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.as.hlsl", L"as_6_5", Shader::Type::as, L"ASMain");
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.ms.hlsl", L"ms_6_5", Shader::Type::ms, L"MSMain");
		shader.CompileShader(L"./Packages/Shader/Render/Grass/Grass.ps.hlsl", L"ps_6_0", Shader::Type::ps, L"PSMain");

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// buffer
		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_ALL, 3, 2); /// CONSTANT_32BIT_DATA
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 2); /// CBV_MATERIAL

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // t0: BladeInstance
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // t1: StartIndex
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // u0: Time
		pipeline_->AddDescriptorRange(2, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // t0: Texture

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // ROOT_PARAM_BLADES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV_START_INDEX
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // ROOT_PARAM_TIME
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 3); // SRV_TEXTURE

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);


		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}

}


void GrassRenderingPipeline::PreDraw(ECSGroup* _ecs, CameraComponent* /*_camera*/, DxCommand* _dxCommand) {
	/// ================================================
	/// 早期リターンの条件チェック
	/// ================================================
	ComponentArray<GrassField>* grassArray = _ecs->GetComponentArray<GrassField>();
	if (!grassArray) {
		return;
	}

	/// 空チェック
	if (grassArray->GetUsedComponents().empty()) {
		return;
	}


	for (auto& grass : grassArray->GetUsedComponents()) {
		/// 草が無効化されている場合はスキップ
		if (!grass->enable || !grass->GetIsCreated()) {
			continue;
		}

		grass->AppendBufferReadCounter(pDxManager_, _dxCommand);
	}
}


void GrassRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// ================================================
	/// 早期リターンの条件チェック
	/// ================================================
	ComponentArray<GrassField>* grassArray = _ecs->GetComponentArray<GrassField>();
	if (!grassArray) {
		return;
	}

	/// 空チェック
	if (grassArray->GetUsedComponents().empty()) {
		return;
	}


	/// ================================================
	/// 描画に必要なデータの準備
	/// ================================================

	/// 描画コマンドをセット
	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	const auto& textures = pAssetCollection_->GetTextures();
	cmdList->SetGraphicsRootDescriptorTable(SRV_TEXTURES, textures[0].GetSRVHandle().gpuHandle);


	/// カメラのBufferを設定
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, CBV_VIEW_PROJECTION);


	for (auto& grass : grassArray->GetUsedComponents()) {
		/// 草が無効化されている場合はスキップ
		if (!grass->enable || !grass->GetIsCreated()) {
			continue;
		}

		UINT instanceCount = static_cast<UINT>(grass->GetInstanceCount());
		if (instanceCount == 0) {
			continue;
		}

		const UINT threadsPerGroup = 32; /// DispatchMesh(1,1,1) あたりのスレッド数
		const UINT grassPerThread = 51; /// 1 スレッドあたりの草の本数
		const UINT oneDrawInstanceCount = grassPerThread * threadsPerGroup; // 3264

		grass->SetupRenderingData(pAssetCollection_);
		grass->StartIndexMapping(oneDrawInstanceCount);

		// SRV/UAVバインド
		grass->GetRwGrassInstanceBuffer().SRVBindForGraphicsCommandList(cmdList, ROOT_PARAM_BLADES);
		grass->GetStartIndexBufferRef().SRVBindForGraphicsCommandList(cmdList, SRV_START_INDEX);
		grass->GetTimeBuffer().UAVBindForGraphicsCommandList(cmdList, ROOT_PARAM_TIME);
		grass->GetMaterialBufferRef().BindForGraphicsCommandList(cmdList, CBV_MATERIAL);


		/// 現在forが1回しか周っていないが、草の数が膨大になった場合ループする回数を計算するようにする
		//for (UINT i = 0; i < bufferCount; ++i) {
		for (UINT i = 0; i < 1; ++i) {
			// 1 DispatchMesh でカバーするグループ数（ceil）
			UINT threadGroupCountX = (instanceCount + oneDrawInstanceCount - 1) / oneDrawInstanceCount;

			UINT constants[2] = { i, threadGroupCountX };
			cmdList->SetGraphicsRoot32BitConstants(CONSTANT_32BIT_DATA, 2, constants, 0); // バッファのインデックスを渡す

			/// DispatchMeshを呼ぶ
			cmdList->DispatchMesh(threadGroupCountX, 1, 1);
		}


	}

}


