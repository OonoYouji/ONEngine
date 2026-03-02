#include "PostProcessFog.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

namespace ONEngine {

void PostProcessFog::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/PostProcess/Screen/Fog/Fog.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // FogParams
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // CameraPos
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SceneColor
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SceneWorldPosition
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // OutputColor
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // SRV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // UAV

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void PostProcessFog::Execute(const std::string& _textureName, DxCommand* _dxCommand, AssetCollection* _assetCollection, EntityComponentSystem* _entityComponentSystem) {

	ECSGroup* currentGroup = _entityComponentSystem->GetCurrentGroup();
	if(!currentGroup) {
		ONEngine::Console::LogError("PostProcessFog::Execute: current ECS group is null");
		return;
	}

	CameraComponent* mainCamera = currentGroup->GetMainCamera();
	if(!mainCamera) {
		ONEngine::Console::LogError("PostProcessFog::Execute: main camera is null");
		return;
	}


	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto cmdList = _dxCommand->GetCommandList();

	mainCamera->GetCameraPosBuffer().BindForComputeCommandList(cmdList, CBV_CAMERA_POS);
	mainCamera->GetFogParamsBuffer().BindForComputeCommandList(cmdList, CBV_FOG_PARAMS);

#ifdef DEBUG_MODE
	if(_textureName.find_last_of("debug") != std::string::npos) {
		if(ECSGroup* debugGroup = _entityComponentSystem->GetECSGroup("Debug")) {
			if(CameraComponent* debugCamera = debugGroup->GetMainCamera()) {
				debugCamera->GetCameraPosBuffer().BindForComputeCommandList(cmdList, CBV_CAMERA_POS);
			}
		}
	}
#endif



	auto& textures = _assetCollection->GetTextures();

	textureIndices_[0] = _assetCollection->GetTextureIndex(_textureName + "Scene");
	textureIndices_[1] = _assetCollection->GetTextureIndex(_textureName + "WorldPosition");
	textureIndices_[2] = _assetCollection->GetTextureIndex("postProcessResult");

	cmdList->SetComputeRootDescriptorTable(SRV_SCENE_COLOR, textures[textureIndices_[0]].GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(SRV_SCENE_WORLD_POSITION, textures[textureIndices_[1]].GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(UAV_OUTPUT_COLOR, textures[textureIndices_[2]].GetUAVGPUHandle());

	cmdList->Dispatch(
		Math::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.x), 16),
		Math::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.y), 16),
		1
	);

	/// 大本のsceneテクスチャに結果をコピー
	CopyResource(
		textures[textureIndices_[2]].GetDxResource().Get(),
		textures[textureIndices_[0]].GetDxResource().Get(),
		cmdList
	);

}

} /// namespace ONEngine