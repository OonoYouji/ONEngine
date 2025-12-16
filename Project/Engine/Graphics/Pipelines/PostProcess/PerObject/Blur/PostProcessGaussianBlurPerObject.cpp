#include "PostProcessGaussianBlurPerObject.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"

void PostProcessGaussianBlurPerObject::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/PostProcess/PerObject/GaussianBlur/GaussianBlur.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// scene tex
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// output tex
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}

	

}

void PostProcessGaussianBlurPerObject::Execute(const std::string& _textureName, DxCommand* _dxCommand, AssetCollection* _assetCollection, EntityComponentSystem* /*_entityComponentSystem*/) {
	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();
	auto& textures = _assetCollection->GetTextures();
	textureIndices_[0] = _assetCollection->GetTextureIndex(_textureName + "Scene");
	textureIndices_[1] = _assetCollection->GetTextureIndex("postProcessResult");

	command->SetComputeRootDescriptorTable(0, textures[textureIndices_[0]].GetSRVGPUHandle());
	command->SetComputeRootDescriptorTable(1, textures[textureIndices_[1]].GetUAVGPUHandle());

	command->Dispatch(
		Math::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.x), 16),
		Math::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.y), 16),
		1
	);


	/// 大本のsceneテクスチャに結果をコピー
	CopyResource(
		textures[textureIndices_[1]].GetDxResource().Get(),
		textures[textureIndices_[0]].GetDxResource().Get(),
		command
	);

}
