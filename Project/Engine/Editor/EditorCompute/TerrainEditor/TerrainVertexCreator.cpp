#include "TerrainVertexCreator.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Asset/Assets/Texture/Texture.h"

using namespace Editor;

TerrainVertexCreator::TerrainVertexCreator() {}
TerrainVertexCreator::~TerrainVertexCreator() {}

void TerrainVertexCreator::Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) {
	pDxManager_ = _dxm;


	{	/// shader

		ONEngine::Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/TerrainVertexCreator.cs.hlsl", L"cs_6_6", ONEngine::Shader::Type::cs);

		pipeline_ = std::make_unique<ONEngine::ComputePipeline>();
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

void TerrainVertexCreator::Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) {
	ONEngine::ComponentArray<ONEngine::Terrain>* terrainArray = _ecs->GetCurrentGroup()->GetComponentArray<ONEngine::Terrain>();
	if (!terrainArray) {
		ONEngine::Console::LogError("TerrainVertexEditorCompute::Execute: Terrain component array is null");
		return;
	}


	ONEngine::Terrain* pTerrain = nullptr;
	for (auto& terrain : terrainArray->GetUsedComponents()) {
		if (!terrain) {
			continue;
		}

		pTerrain = terrain;
	}

	/// terrain がないなら終わり
	if (!pTerrain) {
		ONEngine::Console::LogError("TerrainVertexEditorCompute::Execute: Terrain component is null");
		return;
	}

	/// 未生成の時だけ処理する
	if (!pTerrain->GetIsCreated()) {
		pTerrain->SetIsCreated(true);
		ONEngine::Console::LogInfo("TerrainVertexEditorCompute::Execute: Creating terrain vertices and indices");

		/// VBVとIBVの生成
		pTerrain->CreateVerticesAndIndicesBuffers(pDxManager_->GetDxDevice(), _dxCommand, pDxManager_->GetDxSRVHeap());


		const uint32_t width = static_cast<uint32_t>(pTerrain->GetSize().x);
		const uint32_t depth = static_cast<uint32_t>(pTerrain->GetSize().y);

		/// pipelineに設定&実行
		pipeline_->SetPipelineStateForCommandList(_dxCommand);
		auto cmdList = _dxCommand->GetCommandList();

		terrainSize_.SetMappedData(TerrainSize{ width, depth });
		terrainSize_.BindForComputeCommandList(cmdList, CBV_TERRAIN_SIZE);

		pTerrain->GetRwVertices().UAVBindForComputeCommandList(cmdList, UAV_VERTICES);
		pTerrain->GetRwIndices().UAVBindForComputeCommandList(cmdList, UAV_INDICES);

		const ONEngine::Texture* vertexTexture = _assetCollection->GetTexture("./Packages/Textures/Terrain/TerrainVertex.png");
		const ONEngine::Texture* blendTexture = _assetCollection->GetTexture("./Packages/Textures/Terrain/TerrainSplatBlend.png");

		if (vertexTexture) { cmdList->SetComputeRootDescriptorTable(SRV_VERTEX_TEXTURE, vertexTexture->GetSRVGPUHandle()); }
		if (blendTexture) { cmdList->SetComputeRootDescriptorTable(SRV_SPLAT_BLEND_TEXTURE, blendTexture->GetSRVGPUHandle()); }

		const size_t TGSize = 16; // 16x16のグループサイズ
		cmdList->Dispatch(
			ONEngine::Mathf::DivideAndRoundUp(1000, TGSize), // width
			ONEngine::Mathf::DivideAndRoundUp(1000, TGSize), // depth
			1
		);
	}


}
