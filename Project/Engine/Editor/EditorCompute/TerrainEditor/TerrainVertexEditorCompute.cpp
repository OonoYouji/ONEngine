#include "TerrainVertexEditorCompute.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

TerrainVertexEditorCompute::TerrainVertexEditorCompute() {}
TerrainVertexEditorCompute::~TerrainVertexEditorCompute() {}

void TerrainVertexEditorCompute::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// Shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/TerrainVertexEditor.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); /// CBV_TERRAIN_INFO
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); /// CBV_INPUT_INFO

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_VERTICES
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_POSITION_TEXTURE
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_FLAG_TEXTURE

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_VERTICES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// SRV_POSITION_TEXTURE
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); /// SRV_FLAG_TEXTURE

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}

	{	/// Buffer

		terrainInfo_.Create(_dxManager->GetDxDevice());
		inputInfo_.Create(_dxManager->GetDxDevice());

	}
}

void TerrainVertexEditorCompute::Execute(class EntityComponentSystem* _ecs, DxCommand* _dxCommand, GraphicsResourceCollection* _resourceCollection) {

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
		return;
	}


	/// bufferに値をセット
	terrainInfo_.SetMappedData(TerrainInfo{ pTerrain->GetOwner()->GetId() });

	bool isRaiseTerrainButtonPressed = Input::TriggerMouse(Mouse::Left) && !Input::PressKey(DIK_LSHIFT);
	bool isLowerTerrainButtonPressed = Input::TriggerMouse(Mouse::Left) && Input::PressKey(DIK_LSHIFT);
	int byte = 0;
	byte |= (isRaiseTerrainButtonPressed << 0);
	byte |= (isLowerTerrainButtonPressed << 1);

	inputInfo_.SetMappedData(
		InputInfo{
			Input::GetImGuiImageMousePosition("Scene"),
			10.0f, 0.1f, byte
		}
	);


	/// pipelineの設定&実行

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto cmdList = _dxCommand->GetCommandList();

	/// CBV
	terrainInfo_.BindForComputeCommandList(cmdList, CBV_TERRAIN_INFO);
	inputInfo_.BindForComputeCommandList(cmdList, CBV_INPUT_INFO);

	/// UAV
	pTerrain->GetRwVertices().BindForComputeCommandList(UAV_VERTICES, cmdList);

	/// SRV
	const Texture* positionTexture = _resourceCollection->GetTexture("debugWorldPosition");
	const Texture* flagTexture = _resourceCollection->GetTexture("debugFlags");

	cmdList->SetComputeRootDescriptorTable(SRV_POSITION_TEXTURE, positionTexture->GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(SRV_FLAG_TEXTURE, flagTexture->GetSRVGPUHandle());

	//cmdList->Dispatch(
	//	static_cast<UINT>(EngineConfig::kWindowSize.x / 16.0f),
	//	static_cast<UINT>(EngineConfig::kWindowSize.y / 16.0f),
	//	1
	//);
	const UINT threadGroupSize = 256;
	cmdList->Dispatch(
		(pTerrain->GetMaxVertexNum() + threadGroupSize - 1) / threadGroupSize,
		1, 1
	);
}
