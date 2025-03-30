#include "PostProcessLighting.h"

/// std
#include <list>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Entity/Collection/EntityCollection.h"
#include "Engine/Component/ComputeComponents/Light/Light.h"

PostProcessLighting::PostProcessLighting() {}
PostProcessLighting::~PostProcessLighting() {}


void PostProcessLighting::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	pipeline_ = std::make_unique<ComputePipeline>();

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Assets/Shader/PostProcess/Lighting/Lighting.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 3);

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

void PostProcessLighting::Execute(DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection, EntityCollection* _entityCollection) {

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();

	{	/// set constant buffers
		std::list<DirectionalLight*> directionalLights;
		for (auto& entity : _entityCollection->GetEntities()) {
			auto light = entity->GetComponent<DirectionalLight>();
			if (light) {
				directionalLights.push_back(light);
			}
		}

		directionalLightBufferData_->SetMappingData({ Vector3::kZero, 0.0f, Vector4::kWhite });
		directionalLightBufferData_->BindForComputeCommandList(command, 0);

		cameraBufferData_->SetMappingData({ Vector4::kZero });
		cameraBufferData_->BindForComputeCommandList(command, 1);

	}

	auto& textures = _resourceCollection->GetTextures();
	{	/// set textures

		textureIndices_[0] = _resourceCollection->GetTextureIndex("scene");
		textureIndices_[1] = _resourceCollection->GetTextureIndex("normal");
		textureIndices_[2] = _resourceCollection->GetTextureIndex("worldPosition");
		textureIndices_[3] = _resourceCollection->GetTextureIndex("postProcessResult");

		for (uint32_t index = 0; index < 4; ++index) {
			command->SetComputeRootDescriptorTable(index + 2, textures[textureIndices_[index]]->GetGPUDescriptorHandle());
		}
	}
	
	/// uav textureのbarrier遷移
	textures[textureIndices_[3]]->GetDxResource().CreateBarrier(
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		_dxCommand
	);

	command->Dispatch(1280 / 32, 720 / 32, 1);

	/// uav textureのbarrier遷移
	textures[textureIndices_[3]]->GetDxResource().CreateBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		_dxCommand
	);
}