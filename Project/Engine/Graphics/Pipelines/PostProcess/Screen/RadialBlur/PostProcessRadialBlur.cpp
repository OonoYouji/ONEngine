#include "PostProcessRadialBlur.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/ScreenPostEffectTag/ScreenPostEffectTag.h"

PostProcessRadialBlur::PostProcessRadialBlur() {}
PostProcessRadialBlur::~PostProcessRadialBlur() {}

void PostProcessRadialBlur::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/PostProcess/Screen/RadialBlur/RadialBlur.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// scene tex
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// output tex
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

}

void PostProcessRadialBlur::Execute(const std::string& _textureName, DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection, EntityComponentSystem* _entityComponentSystem) {

	ScreenPostEffectTag* tag = nullptr;
	for (auto& entity : _entityComponentSystem->GetGameECSGroup()->GetEntities()) {
		tag = entity->GetComponent<ScreenPostEffectTag>();
		if (tag) {
			break;
		}
	}

	if (!tag || !tag->GetPostEffectEnable(PostEffectType_RadialBlur)) {
		return; // ラジアルブラーエフェクトが無効な場合は何もしない
	}

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();
	auto& textures = _resourceCollection->GetTextures();
	textureIndices_[0] = _resourceCollection->GetTextureIndex(_textureName + "Scene");
	textureIndices_[1] = _resourceCollection->GetTextureIndex("postProcessResult");

	command->SetComputeRootDescriptorTable(0, textures[textureIndices_[0]]->GetSRVGPUHandle());
	command->SetComputeRootDescriptorTable(1, textures[textureIndices_[1]]->GetUAVGPUHandle());

	command->Dispatch(
		static_cast<UINT>(EngineConfig::kWindowSize.x) / 16,
		static_cast<UINT>(EngineConfig::kWindowSize.y) / 16,
		1
	);

	CopyResource(
		textures[textureIndices_[1]]->GetDxResource().Get(),
		textures[textureIndices_[0]]->GetDxResource().Get(),
		command
	);
}
