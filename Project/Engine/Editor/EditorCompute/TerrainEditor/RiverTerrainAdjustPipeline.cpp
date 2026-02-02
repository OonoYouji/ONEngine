#include "RiverTerrainAdjustPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Array/ComponentArray.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

using namespace Editor;

RiverTerrainAdjustPipeline::RiverTerrainAdjustPipeline() = default;
RiverTerrainAdjustPipeline::~RiverTerrainAdjustPipeline() = default;

void RiverTerrainAdjustPipeline::Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) {

	{	/// shader
		ONEngine::Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/RiverTerrainAdjust.cs.hlsl", L"cs_6_6", ONEngine::Shader::Type::cs);

		pipeline_ = std::make_unique<ONEngine::ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_PARAMS

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_TERRAIN_VERTICES
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// SRV_RIVER_VERTICES
		pipeline_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// SRV_RIVER_INDICES

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}
}

void RiverTerrainAdjustPipeline::Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* /*_assetCollection*/) {

	/// ----------------------------------------------------
	/// 早期 return条件
	/// ----------------------------------------------------
	ONEngine::ComponentArray<ONEngine::Terrain>* terrainArray = _ecs->GetCurrentGroup()->GetComponentArray<ONEngine::Terrain>();
	if (!terrainArray) {
		return;
	}

	if (terrainArray->GetUsedComponents().empty()) {
		return;
	}

	ONEngine::Terrain* terrain = terrainArray->GetUsedComponents().front();
	if (!terrain) {
		return;
	}

	/// 川の情報を取得
	ONEngine::River* river = terrain->GetRiver();
	if (!river->GetIsCreatedBuffers() || !river->GetIsGenerateMeshRequest()) {
		return;
	}


	/// ----------------------------------------------------
	/// pipeline起動に必要な情報を取得
	/// ----------------------------------------------------

	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);


	river->GetParamBuffer().BindForComputeCommandList(cmdList, CBV_PARAMS);
	terrain->GetRwVertices().UAVBindForComputeCommandList(cmdList, UAV_TERRAIN_VERTICES);
	river->GetRwVertices().UAVBindForComputeCommandList(cmdList, SRV_RIVER_VERTICES);
	river->GetRwIndices().UAVBindForComputeCommandList(cmdList, SRV_RIVER_INDICES);

	const UINT maxVertex = static_cast<UINT>(terrain->GetMaxVertexNum());
	const UINT groupSize = 32;
	cmdList->Dispatch(ONEngine::Math::DivideAndRoundUp(maxVertex, groupSize), 1, 1);

}

