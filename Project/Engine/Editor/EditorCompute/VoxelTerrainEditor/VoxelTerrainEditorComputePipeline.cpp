#include "VoxelTerrainEditorComputePipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"



VoxelTerrainEditorComputePipeline::VoxelTerrainEditorComputePipeline() = default;
VoxelTerrainEditorComputePipeline::~VoxelTerrainEditorComputePipeline() = default;

void VoxelTerrainEditorComputePipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/VoxelTerrainEditor.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		/// CBV
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // CBV_TERRAIN_INFO
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2); // CBV_CAMERA
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3); // CBV_INPUT_INFO

		/// Descriptor Range
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CHUNKS
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_WORLD_TEXTURE
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_VOXEL_TEXTURES

		/// SRV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // SRV_CHUNKS
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV_WORLD_TEXTURE

		/// UAV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // UAV_VOXEL_TEXTURES

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void VoxelTerrainEditorComputePipeline::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, AssetCollection* _assetCollection) {
}
