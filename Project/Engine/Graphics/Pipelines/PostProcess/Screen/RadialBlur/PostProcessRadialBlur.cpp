#include "PostProcessRadialBlur.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/RendererComponents/ScreenPostEffectTag/ScreenPostEffectTag.h"

PostProcessRadialBlur::PostProcessRadialBlur() {}
PostProcessRadialBlur::~PostProcessRadialBlur() {}

void PostProcessRadialBlur::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

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
		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}

}

void PostProcessRadialBlur::Execute(const std::string& _textureName, DxCommand* _dxCommand, AssetCollection* _assetCollection, EntityComponentSystem* _entityComponentSystem) {

	/// 配列の取得とタグの確認
	ComponentArray<ScreenPostEffectTag>* screenPostEffectTagArray = _entityComponentSystem->GetCurrentGroup()->GetComponentArray<ScreenPostEffectTag>();
	if (!screenPostEffectTagArray || screenPostEffectTagArray->GetUsedComponents().empty()) {
		return;
	}

	ScreenPostEffectTag* tag = nullptr;
	for (auto& comp : screenPostEffectTagArray->GetUsedComponents()) {
		if (!comp || !comp->enable) {
			continue;
		}

		tag = comp;
		break;
	}

	if (!tag || !tag->GetPostEffectEnable(PostEffectType_RadialBlur)) {
		return; // ラジアルブラーエフェクトが無効な場合は何もしない
	}

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto command = _dxCommand->GetCommandList();
	auto& textures = _assetCollection->GetTextures();
	textureIndices_[0] = _assetCollection->GetTextureIndex(_textureName + "Scene");
	textureIndices_[1] = _assetCollection->GetTextureIndex("postProcessResult");

	command->SetComputeRootDescriptorTable(0, textures[textureIndices_[0]].GetSRVGPUHandle());
	command->SetComputeRootDescriptorTable(1, textures[textureIndices_[1]].GetUAVGPUHandle());

	command->Dispatch(
		Mathf::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.x), 16),
		Mathf::DivideAndRoundUp(static_cast<uint32_t>(EngineConfig::kWindowSize.y), 16),
		1
	);

	CopyResource(
		textures[textureIndices_[1]].GetDxResource().Get(),
		textures[textureIndices_[0]].GetDxResource().Get(),
		command
	);
}
