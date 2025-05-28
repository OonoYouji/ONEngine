#include "PostProcessGrayscale.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

void PostProcessGrayscale::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/PostProcess/Screen/Grayscale/Grayscale.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->CreatePipeline(_dxManager->GetDxDevice());


	}

}

void PostProcessGrayscale::Execute(const std::string& _textureName, DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection, EntityComponentSystem* _entityComponentSystem) {
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();
	auto& textures = _resourceCollection->GetTextures();
	textureIndices_[0] = _resourceCollection->GetTextureIndex(_textureName);
	textureIndices_[1] = _resourceCollection->GetTextureIndex("postProcessResult");

	{
		command->SetComputeRootDescriptorTable(0, textures[textureIndices_[0]]->GetSRVGPUHandle());
		command->SetComputeRootDescriptorTable(1, textures[textureIndices_[1]]->GetUAVGPUHandle());

		command->Dispatch(1280 / 16, 720 / 16, 1);
	}


	/// 大本のsceneテクスチャに結果をコピー

	/// resource barrier
	CD3DX12_RESOURCE_BARRIER uavTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[1]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_SOURCE
	);

	CD3DX12_RESOURCE_BARRIER sceneTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[0]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	command->ResourceBarrier(1, &uavTexBarrier);
	command->ResourceBarrier(1, &sceneTexBarrier);


	/// copy
	command->CopyResource(
		textures[textureIndices_[0]]->GetDxResource().Get(),
		textures[textureIndices_[1]]->GetDxResource().Get()
	);


	/// resource barrier
	uavTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[1]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	sceneTexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		textures[textureIndices_[0]]->GetDxResource().Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);


	command->ResourceBarrier(1, &uavTexBarrier);
	command->ResourceBarrier(1, &sceneTexBarrier);
}
