#include "VoxelTerrainTransvoxelRenderingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS//EntityComponentSystem/EntityComponentSystem.h"


using namespace ONEngine;

VoxelTerrainTransvoxelRenderingPipeline::VoxelTerrainTransvoxelRenderingPipeline(AssetCollection* _ac)
	: pAssetCollection_(_ac) {
}

VoxelTerrainTransvoxelRenderingPipeline::~VoxelTerrainTransvoxelRenderingPipeline() = default;

void VoxelTerrainTransvoxelRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/Transvoxel.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/Transvoxel.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/Transvoxel.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // VoxelTerrainInfo
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // ViewProjection
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2); // CameraPosition
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3); // Material

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Chunk array
		pipeline_->AddDescriptorRange(1, MAX_TEXTURE_COUNT * 2, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // VoxelTerrain Texture3D

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // Chunk array
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // VoxelTerrain Texture3D


		pipeline_->AddStaticSampler(StaticSampler::ClampSampler(), D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());
		
		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}
}

void VoxelTerrainTransvoxelRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {
}