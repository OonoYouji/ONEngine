#include "PostProcessShadowApply.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"


void PostProcessShadowApply::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/PostProcess/Screen/ShadowMap/ShadowMap.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		/// pipeline
		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		/// root signature
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, ROOT_PARAM::CBV_VIEW_PROJECTION);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, ROOT_PARAM::CBV_SHADOW_PARAM);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_SCENE_COLOR
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_SCENE_DEPTH
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_SHADOW_MAP
		pipeline_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_WORLD_POSITION
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_OUTPUT_COLOR

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 3);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 4);

		/// sampler 
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 1, true);


		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}

	{	/// buffer
		shadowParamBuffer_.Create(_dxm->GetDxDevice());

		/// 初期化用に適当な値をセット
		shadowParamBuffer_.SetMappedData(ShadowParameter{
			.screenSize = EngineConfig::kWindowSize,
			.texelSizeShadow = Vector2(1.0f / EngineConfig::kWindowSize.x, 1.0f / EngineConfig::kWindowSize.y),
			.shadowBias = 0.001f,
			.shadowDarkness = 0.7f,
			.pcfRadius = 2,
			});
	}
}

void PostProcessShadowApply::Execute(const std::string& _textureName, DxCommand* _dxCommand, AssetCollection* _assetCollection, EntityComponentSystem* _ecs) {

	/// ---------------------------------------------------
	/// 現在のGroupからShadowCasterを取得
	/// ---------------------------------------------------
	ECSGroup* currentGroup = _ecs->GetCurrentGroup();
	if (!currentGroup) {
		return;
	}


	ComponentArray<ShadowCaster>* shadowCasterArray = currentGroup->GetComponentArray<ShadowCaster>();
	if (!shadowCasterArray || shadowCasterArray->GetUsedComponents().empty()) {
		return;
	}


	/// 使用できる状態のShadowCasterを取得
	ShadowCaster* shadowCaster = nullptr;
	for (auto& sc : shadowCasterArray->GetUsedComponents()) {
		if (sc && sc->enable) {
			shadowCaster = sc;
			break;
		}
	}

	if (!shadowCaster) {
		return;
	}


	CameraComponent* shadowCamera = shadowCaster->GetShadowCasterCamera();
	if (!shadowCamera) {
		return;
	}


	/// ---------------------------------------------------
	/// PipelineとBufferの設定
	/// ---------------------------------------------------


	/// --------------- pipelineの設定 --------------- ///
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto cmdList = _dxCommand->GetCommandList();


	/// --------------- bufferの設定 --------------- ///
	shadowCamera->GetViewProjectionBuffer().BindForComputeCommandList(
		cmdList, ROOT_PARAM::CBV_VIEW_PROJECTION
	);

	shadowParamBuffer_.BindForComputeCommandList(
		cmdList, ROOT_PARAM::CBV_SHADOW_PARAM
	);


	/// --------------- 格テクスチャの設定 --------------- ///

	// シーンカラー
	Texture* sceneColorTex = _assetCollection->GetTexture(_textureName + "Scene");
	cmdList->SetComputeRootDescriptorTable(
		ROOT_PARAM::SRV_SCENE_COLOR,
		sceneColorTex->GetSRVHandle().gpuHandle
	);

}
