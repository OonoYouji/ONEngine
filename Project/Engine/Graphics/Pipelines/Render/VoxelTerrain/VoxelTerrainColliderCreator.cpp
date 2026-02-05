#include "VoxelTerrainColliderCreator.h"


/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"

namespace ONEngine {

VoxelTerrainColliderCreator::VoxelTerrainColliderCreator() {

}

void VoxelTerrainColliderCreator::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/ChunkColliderCreate.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		computePipeline_ = std::make_unique<ComputePipeline>();
		computePipeline_->SetShader(&shader);

		computePipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // VOXEL_TERRAIN_INFO
		computePipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // GENERATE_PARAMS
		computePipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_ALL, 2); // CHUNK_ID_C32BIT

		computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_VERTICES
		computePipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CHUNKS
		computePipeline_->AddDescriptorRange(2, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CHUNK_TEXTURE3D

		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // UAV_VERTICES
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV_CHUNKS
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // SRV_CHUNK_TEXTURE3D

		computePipeline_->AddStaticSampler(StaticSampler::ClampSampler(), D3D12_SHADER_VISIBILITY_ALL, 0);

		computePipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void VoxelTerrainColliderCreator::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {
}

}
