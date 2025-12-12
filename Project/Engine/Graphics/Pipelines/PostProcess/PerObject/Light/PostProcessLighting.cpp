#include "PostProcessLighting.h"

using namespace ONEngine;

/// std
#include <list>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

PostProcessLighting::PostProcessLighting() {}
PostProcessLighting::~PostProcessLighting() {}


void PostProcessLighting::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	pipeline_ = std::make_unique<ComputePipeline>();

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/PostProcess/PerObject/Lighting/Lighting.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(4, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 3);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 4);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 5);

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}


	{
		/// constant buffer
		directionalLightBufferData_ = std::make_unique<ConstantBuffer<DirectionalLightBufferData>>();
		directionalLightBufferData_->Create(_dxm->GetDxDevice());

		cameraBufferData_ = std::make_unique<ConstantBuffer<CameraBufferData>>();
		cameraBufferData_->Create(_dxm->GetDxDevice());

	}

}

void PostProcessLighting::Execute(const std::string& _textureName, DxCommand* _dxCommand, AssetCollection* _assetCollection, EntityComponentSystem* _pEntityComponentSystem) {

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();
	auto& textures = _assetCollection->GetTextures();

	{	/// set constant buffers

		ECSGroup* ecsGroup = _pEntityComponentSystem->GetCurrentGroup();

		std::list<DirectionalLight*> directionalLights;
		for (auto& entity : ecsGroup->GetEntities()) {
			auto light = entity->GetComponent<DirectionalLight>();
			if (light) {
				directionalLights.push_back(light);
			}
		}

		///!< check light is empty?
		if (directionalLights.empty()) {
			return;
		}

		/// set light data
		directionalLightBufferData_->SetMappedData(
			{
				Mathf::ConvertToVector4(directionalLights.front()->GetOwner()->GetPosition(), 1.0f),
				directionalLights.front()->GetColor(),
				directionalLights.front()->GetDirection(),
				directionalLights.front()->GetIntensity()
			}
		);
		directionalLightBufferData_->BindForComputeCommandList(command, 0);

		CameraComponent* camera = ecsGroup->GetMainCamera();
		if (camera) {
			if (GameEntity* entity = camera->GetOwner()) {
				cameraBufferData_->SetMappedData({ Mathf::ConvertToVector4(entity->GetPosition(), 1.0f) });
			}
		}

		cameraBufferData_->BindForComputeCommandList(command, 1);
	}


	enum {
		SCENE,
		WORLD_POSITION,
		NORMAL,
		FLAGS,
		SKYBOX,
		POST_PROCESS_RESULT,
	};


	{	/// set textures

		textureIndices_[0] = _assetCollection->GetTextureIndex(_textureName + "Scene");
		textureIndices_[1] = _assetCollection->GetTextureIndex(_textureName + "WorldPosition");
		textureIndices_[2] = _assetCollection->GetTextureIndex(_textureName + "Normal");
		textureIndices_[3] = _assetCollection->GetTextureIndex(_textureName + "Flags");

		textureIndices_[4] = _assetCollection->GetTextureIndex("./Packages/Textures/kloofendal_48d_partly_cloudy_puresky_2k.dds");
		textureIndices_[5] = _assetCollection->GetTextureIndex("postProcessResult");

		for (uint32_t index = 0; index < 5; ++index) {
			command->SetComputeRootDescriptorTable(index + 2, textures[textureIndices_[index]].GetSRVGPUHandle());
		}

		command->SetComputeRootDescriptorTable(7, textures[textureIndices_[POST_PROCESS_RESULT]].GetUAVGPUHandle());
	}


	command->Dispatch(
		Mathf::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.x), 16),
		Mathf::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.y), 16),
		1
	);

	/// 大本のsceneテクスチャに結果をコピー
	CopyResource(
		textures[textureIndices_[POST_PROCESS_RESULT]].GetDxResource().Get(),
		textures[textureIndices_[SCENE]].GetDxResource().Get(),
		command
	);



}
