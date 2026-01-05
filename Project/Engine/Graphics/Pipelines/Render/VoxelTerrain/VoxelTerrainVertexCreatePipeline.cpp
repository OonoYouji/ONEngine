#include "VoxelTerrainVertexCreatePipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"

using namespace ONEngine;

VoxelTerrainVertexCreatePipeline::VoxelTerrainVertexCreatePipeline(AssetCollection* _ac) : pAssetCollection_(_ac) {}
VoxelTerrainVertexCreatePipeline::~VoxelTerrainVertexCreatePipeline() {}


void VoxelTerrainVertexCreatePipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrainTest/VoxelTerrainVertexCreator.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		computePipeline_ = std::make_unique<ComputePipeline>();
		computePipeline_->SetShader(&shader);

		/// CBV
		computePipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // CBV_VOXEL_TERRAIN_INFO
		computePipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // CBV_MARCHING_CUBE

		/// 32bit constant
		computePipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_ALL, 2); // BIT32_CHUNK_INDEX

		/// Descriptor Range
		computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CHUNKS
		computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // APPEND_OUT_VERTICES
		computePipeline_->AddDescriptorRange(1, MAX_TEXTURE_COUNT * 2, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_VOXEL_TEXTURES

		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // SRV_CHUNKS
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // APPEND_OUT_VERTICES
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // SRV_VOXEL_TEXTURES

		computePipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		computePipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void VoxelTerrainVertexCreatePipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<VoxelTerrain>* voxelTerrainCompArray = _ecs->GetComponentArray<VoxelTerrain>();
	if(!CheckComponentArrayEnable(voxelTerrainCompArray)) {
		return;
	}


}