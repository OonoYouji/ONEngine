#include "VoxelTerrainVertexCreatePipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/Core/Utility/Utility.h"

using namespace ONEngine;

VoxelTerrainVertexCreatePipeline::VoxelTerrainVertexCreatePipeline(AssetCollection* _ac) : pAssetCollection_(_ac) {}
VoxelTerrainVertexCreatePipeline::~VoxelTerrainVertexCreatePipeline() {}


void VoxelTerrainVertexCreatePipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	pDxManager_ = _dxm;

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
		computePipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // APPEND_OUT_VERTICES
		computePipeline_->AddDescriptorRange(2, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_VOXEL_TEXTURES

		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // SRV_CHUNKS
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // APPEND_OUT_VERTICES
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // SRV_VOXEL_TEXTURES

		//computePipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		computePipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void VoxelTerrainVertexCreatePipeline::PreDraw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<VoxelTerrain>* voxelTerrainCompArray = _ecs->GetComponentArray<VoxelTerrain>();
	if(!CheckComponentArrayEnable(voxelTerrainCompArray)) {
		return;
	}


	VoxelTerrain* vt = nullptr;
	for(auto& voxelTerrain : voxelTerrainCompArray->GetUsedComponents()) {
		if(CheckComponentEnable(voxelTerrain)) {
			vt = voxelTerrain;
			break;
		}
	}

	if(!CheckComponentEnable(vt)) {
		return;
	}

	if(vt->isCreatedVoxelTerrain_) {
		return;
	}

	if(!vt->cBufferTerrainInfo_.Get() ||
	   !vt->sBufferChunks_.GetResource().Get()) {
		return;
	}

	if(!vt->chunks_[0].rwVertices.GetResource().Get()) {
		return;
	}


	if(!vt->cBufferMarchingCubeInfo_.Get()) {
		vt->cBufferMarchingCubeInfo_.Create(pDxManager_->GetDxDevice());
		vt->cBufferMarchingCubeInfo_.SetMappedData(
			{ .isoValue = 1.0f, .voxelSize = 1.0f }
		);
	}


	computePipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto cmdList = _dxCommand->GetCommandList();

	vt->cBufferTerrainInfo_.BindForComputeCommandList(cmdList, CBV_VOXEL_TERRAIN_INFO);
	vt->cBufferMarchingCubeInfo_.BindForComputeCommandList(cmdList, CBV_MARCHING_CUBE);
	vt->sBufferChunks_.SRVBindForComputeCommandList(cmdList, SRV_CHUNKS);

	D3D12_GPU_DESCRIPTOR_HANDLE frontSRVHandle = pDxManager_->GetDxSRVHeap()->GetSRVStartGPUHandle();
	cmdList->SetComputeRootDescriptorTable(SRV_VOXEL_TEXTURES, frontSRVHandle);

	for(uint32_t i = 0; i < vt->chunks_.size(); i++) {
		const auto& chunk = vt->chunks_[i];
		cmdList->SetComputeRoot32BitConstant(BIT32_CHUNK_INDEX, i, 0);
		chunk.rwVertices.AppendBindForComputeCommandList(cmdList, APPEND_OUT_VERTICES);

		const Vector3Int& chunkSize = vt->GetChunkSize();
		const Vector3Int numthreads = { 8, 8, 8 };
		cmdList->Dispatch(
			Math::DivideAndRoundUp(chunkSize.x, numthreads.x),
			Math::DivideAndRoundUp(chunkSize.y, numthreads.y),
			Math::DivideAndRoundUp(chunkSize.z, numthreads.z)
		);
	}

	/// カウンター
	for(uint32_t i = 0; i < vt->chunks_.size(); i++) {
		auto& chunk = vt->chunks_[i];
		uint32_t count = chunk.rwVertices.ReadCounter(_dxCommand);
		chunk.vertexCount = count;
	}

	pDxManager_->HeapBindToCommandList();
	vt->isCreatedVoxelTerrain_ = true;

}