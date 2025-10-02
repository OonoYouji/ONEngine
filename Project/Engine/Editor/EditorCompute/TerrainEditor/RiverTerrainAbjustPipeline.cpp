#include "RiverTerrainAbjustPipeline.h"


/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

RiverTerrainAbjustPipeline::RiverTerrainAbjustPipeline() = default;
RiverTerrainAbjustPipeline::~RiverTerrainAbjustPipeline() = default;

void RiverTerrainAbjustPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/RiverTerrainAbjust.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_PARAMS

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_TERRAIN_VERTICES
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// SRV_RIVER_VERTICES
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// SRV_RIVER_INDICES

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}
}

void RiverTerrainAbjustPipeline::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection) {

	/// ----------------------------------------------------
	/// 早期return条件
	/// ----------------------------------------------------
	ComponentArray<Terrain>* terrainArray = _ecs->GetCurrentGroup()->GetComponentArray<Terrain>();
	if (!terrainArray) {
		return;
	}

	if(terrainArray->GetUsedComponents().empty()) {
		return;
	}

	Terrain* terrain = terrainArray->GetUsedComponents().front();
	if(!terrain) {
		return;
	}


	/// 川の情報を取得
	River* river = terrain->GetRiver();
	if (!river->GetIsCreatedBuffers() || !river->GetIsGenerateMeshRequest()) {
		return;
	}


	/// ----------------------------------------------------
	/// pipeline起動に必要な情報を取得
	/// ----------------------------------------------------

	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);


	river->GetParamBufRef().BindForComputeCommandList(cmdList, CBV_PARAMS);
	terrain->GetRwVertices().UAVBindForComputeCommandList(cmdList, UAV_TERRAIN_VERTICES);
	river->GetRwVerticesRef().UAVBindForComputeCommandList(cmdList, SRV_RIVER_VERTICES);
	river->GetRwIndicesRef().UAVBindForComputeCommandList(cmdList, SRV_RIVER_INDICES);

	UINT maxVertex = static_cast<UINT>(terrain->GetMaxVertexNum());
	UINT groupSize = 32;
	UINT dispatchCount = (maxVertex + groupSize - 1) / groupSize;
	cmdList->Dispatch(
		dispatchCount,
		1,
		1
	);

}

