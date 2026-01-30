#include "VoxelTerrainBrushPreviewRenderingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

namespace ONEngine {

VoxelTerrainBrushPreviewRenderingPipeline::VoxelTerrainBrushPreviewRenderingPipeline(AssetCollection* assetCollection)
	: pAssetCollection_(assetCollection) {
}
VoxelTerrainBrushPreviewRenderingPipeline::~VoxelTerrainBrushPreviewRenderingPipeline() = default;

void VoxelTerrainBrushPreviewRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// shader 
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/BrushPreview.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/BrushPreview.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/BrushPreview.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// CBV
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 4);

		/// SRV
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(1, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);

		pipeline_->AddStaticSampler(StaticSampler::ClampSampler(), D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}
}

void VoxelTerrainBrushPreviewRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

}


} /// namespace ONEngine