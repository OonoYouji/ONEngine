#include "TerrainVertexCreator.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"


TerrainVertexCreator::TerrainVertexCreator() {}
TerrainVertexCreator::~TerrainVertexCreator() {}

void TerrainVertexCreator::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {
	pDxManager_ = _dxManager;


	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/TerrainVertexCreator.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_VERTICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_VERTICES

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{	/// buffer

	}

}

void TerrainVertexCreator::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection) {
	ComponentArray<Terrain>* terrainArray = _ecs->GetComponentArray<Terrain>();
	if (!terrainArray) {
		Console::LogError("TerrainVertexEditorCompute::Execute: Terrain component array is null");
		return;
	}


	Terrain* pTerrain = nullptr;
	for (auto& terrain : terrainArray->GetUsedComponents()) {
		if (!terrain) {
			continue;
		}

		pTerrain = terrain;
	}

	/// terrain がないなら終わり
	if (!pTerrain) {
		return;
	}


	if (!pTerrain->GetIsCreated()) {
		pTerrain->SetIsCreated(true);

		pTerrain->GetRwVertices().CreateUAV(
			sizeof(TerrainVertex) * 1000,
			pDxManager_->GetDxDevice(), _dxCommand, pDxManager_->GetDxSRVHeap()
		);
	}


	/// pipelineに設定&実行

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto cmdList = _dxCommand->GetCommandList();

	pTerrain->GetRwVertices().BindForComputeCommandList(
		UAV_VERTICES, cmdList
	);

	cmdList->Dispatch(
		static_cast<UINT>(EngineConfig::kWindowSize.x / 16.0f),
		static_cast<UINT>(EngineConfig::kWindowSize.y / 16.0f),
		1
	);
}
