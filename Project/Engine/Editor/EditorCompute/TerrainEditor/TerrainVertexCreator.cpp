#include "TerrainVertexCreator.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Asset/Assets/Texture/Texture.h"

TerrainVertexCreator::TerrainVertexCreator() {}
TerrainVertexCreator::~TerrainVertexCreator() {}

void TerrainVertexCreator::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	pDxManager_ = _dxm;


	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/TerrainVertexCreator.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_VERTICES
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_INDICES
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_VERTEX_TEXTURE
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_SPLAT_BLEND_TEXTURE
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_VERTICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// UAV_INDICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); /// SRV_VERTEX_TEXTURE
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 3); /// SRV_SPLAT_BLEND_TEXTURE

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}

	{	/// buffer
		terrainSize_.Create(_dxm->GetDxDevice());
	}


}

void TerrainVertexCreator::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, AssetCollection* _assetCollection) {
	ComponentArray<Terrain>* terrainArray = _ecs->GetCurrentGroup()->GetComponentArray<Terrain>();
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
		Console::LogError("TerrainVertexEditorCompute::Execute: Terrain component is null");
		return;
	}

	/// 未生成の時だけ処理する
	if (!pTerrain->GetIsCreated()) {
		pTerrain->SetIsCreated(true);
		Console::LogInfo("TerrainVertexEditorCompute::Execute: Creating terrain vertices and indices");

		pTerrain->GetRwVertices().CreateUAV(
			pTerrain->GetMaxVertexNum(),
			pDxManager_->GetDxDevice(), _dxCommand, pDxManager_->GetDxSRVHeap()
		);

		pTerrain->GetRwIndices().CreateUAV(
			pTerrain->GetMaxIndexNum(),
			pDxManager_->GetDxDevice(), _dxCommand, pDxManager_->GetDxSRVHeap()
		);


		const uint32_t width = static_cast<uint32_t>(pTerrain->GetSize().x);
		const uint32_t depth = static_cast<uint32_t>(pTerrain->GetSize().y);

		/// pipelineに設定&実行
		pipeline_->SetPipelineStateForCommandList(_dxCommand);
		auto cmdList = _dxCommand->GetCommandList();

		terrainSize_.SetMappedData(TerrainSize{ width, depth });
		terrainSize_.BindForComputeCommandList(cmdList, CBV_TERRAIN_SIZE);

		pTerrain->GetRwVertices().UAVBindForComputeCommandList(cmdList, UAV_VERTICES);
		pTerrain->GetRwIndices().UAVBindForComputeCommandList(cmdList, UAV_INDICES);

		const Texture* vertexTexture = _assetCollection->GetTexture("./Packages/Textures/Terrain/TerrainVertex.png");
		const Texture* blendTexture = _assetCollection->GetTexture("./Packages/Textures/Terrain/TerrainSplatBlend.png");

		if (vertexTexture) { cmdList->SetComputeRootDescriptorTable(SRV_VERTEX_TEXTURE, vertexTexture->GetSRVGPUHandle()); }
		if (blendTexture) { cmdList->SetComputeRootDescriptorTable(SRV_SPLAT_BLEND_TEXTURE, blendTexture->GetSRVGPUHandle()); }

		const size_t TGSize = 16; // 16x16のグループサイズ
		cmdList->Dispatch(
			(1000 + TGSize - 1) / TGSize, // width
			(1000 + TGSize - 1) / TGSize, // depth
			1
		);
	}


}
