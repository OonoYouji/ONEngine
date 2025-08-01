#include "PostProcessLighting.h"

/// std
#include <list>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"

PostProcessLighting::PostProcessLighting() {}
PostProcessLighting::~PostProcessLighting() {}


void PostProcessLighting::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
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

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}


	{
		/// constant buffer
		directionalLightBufferData_ = std::make_unique<ConstantBuffer<DirectionalLightBufferData>>();
		directionalLightBufferData_->Create(_dxManager->GetDxDevice());

		cameraBufferData_ = std::make_unique<ConstantBuffer<CameraBufferData>>();
		cameraBufferData_->Create(_dxManager->GetDxDevice());

	}

}

void PostProcessLighting::Execute(const std::string& _textureName, DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection, EntityComponentSystem* _pEntityComponentSystem) {

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();
	auto& textures = _resourceCollection->GetTextures();

	{	/// set constant buffers
		std::list<DirectionalLight*> directionalLights;
		for (auto& entity : _pEntityComponentSystem->GetEntities()) {
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
				Vector4::Convert(directionalLights.front()->GetOwner()->GetPosition()),
				directionalLights.front()->GetColor(),
				directionalLights.front()->GetDirection(),
				directionalLights.front()->GetIntensity()
			}
		);
		directionalLightBufferData_->BindForComputeCommandList(command, 0);

		Camera* camera = _pEntityComponentSystem->GetDebugCamera();
		cameraBufferData_->SetMappedData({ Vector4(camera->GetPosition(), 1.0f) });
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

		//auto& textures = _resourceCollection->GetTextures();

		textureIndices_[1] = _resourceCollection->GetTextureIndex(_textureName + "WorldPosition");
		textureIndices_[2] = _resourceCollection->GetTextureIndex(_textureName + "Normal");
		textureIndices_[3] = _resourceCollection->GetTextureIndex(_textureName + "Flags");

		textureIndices_[0] = _resourceCollection->GetTextureIndex(_textureName + "Scene");
		textureIndices_[4] = _resourceCollection->GetTextureIndex("./Packages/Textures/kloofendal_48d_partly_cloudy_puresky_2k.dds");
		textureIndices_[5] = _resourceCollection->GetTextureIndex("postProcessResult");

		for (uint32_t index = 0; index < 5; ++index) {
			command->SetComputeRootDescriptorTable(index + 2, textures[textureIndices_[index]]->GetSRVGPUHandle());
		}

		command->SetComputeRootDescriptorTable(7, textures[textureIndices_[POST_PROCESS_RESULT]]->GetUAVGPUHandle());
	}


	command->Dispatch(
		static_cast<UINT>(EngineConfig::kWindowSize.x) / 16,
		static_cast<UINT>(EngineConfig::kWindowSize.y) / 16,
		1
	);

	/// 大本のsceneテクスチャに結果をコピー

	/// resource barrier
	CD3DX12_RESOURCE_BARRIER uavTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[POST_PROCESS_RESULT]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);

	CD3DX12_RESOURCE_BARRIER sceneTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[SCENE]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	command->ResourceBarrier(1, &uavTexBarrier);
	command->ResourceBarrier(1, &sceneTexBarrier);


	/// copy
	command->CopyResource(
		textures[textureIndices_[SCENE]]->GetDxResource().Get(),
		textures[textureIndices_[POST_PROCESS_RESULT]]->GetDxResource().Get()
	);


	/// resource barrier
	uavTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[POST_PROCESS_RESULT]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	sceneTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[SCENE]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);


	command->ResourceBarrier(1, &uavTexBarrier);
	command->ResourceBarrier(1, &sceneTexBarrier);




}