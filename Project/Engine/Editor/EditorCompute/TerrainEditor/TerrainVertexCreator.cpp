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

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_VERTICES
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_INDICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_VERTICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// UAV_INDICES

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

	{	/// buffer
		terrainSize_.Create(_dxManager->GetDxDevice());
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

	/// 未生成の時だけ処理する
	if (!pTerrain->GetIsCreated()) {
		pTerrain->SetIsCreated(true);

		pTerrain->GetRwVertices().CreateUAV(
			sizeof(TerrainVertex) * pTerrain->GetMaxVertexNum(),
			pDxManager_->GetDxDevice(), _dxCommand, pDxManager_->GetDxSRVHeap()
		);

		pTerrain->GetRwIndices().CreateUAV(
			sizeof(uint32_t) * pTerrain->GetMaxIndexNum(),
			pDxManager_->GetDxDevice(), _dxCommand, pDxManager_->GetDxSRVHeap()
		);


		/// pipelineに設定&実行
		pipeline_->SetPipelineStateForCommandList(_dxCommand);
		auto cmdList = _dxCommand->GetCommandList();

		terrainSize_.SetMappedData(TerrainSize{
			.width = static_cast<uint32_t>(pTerrain->GetSize().x),
			.height = static_cast<uint32_t>(pTerrain->GetSize().x),
			});
		terrainSize_.BindForComputeCommandList(cmdList, CBV_TERRAIN_SIZE);

		pTerrain->GetRwVertices().BindForComputeCommandList(
			UAV_VERTICES, cmdList);
		pTerrain->GetRwIndices().BindForComputeCommandList(
			UAV_INDICES, cmdList);

		cmdList->Dispatch(
			static_cast<UINT>(EngineConfig::kWindowSize.x / 16.0f),
			static_cast<UINT>(EngineConfig::kWindowSize.y / 16.0f),
			1
		);
	}
}
