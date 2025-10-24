#include "TerrainVertexEditorCompute.h"

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Asset/Collection/AssetCollection.h"

#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"

TerrainVertexEditorCompute::TerrainVertexEditorCompute() = default;
TerrainVertexEditorCompute::~TerrainVertexEditorCompute() = default;

void TerrainVertexEditorCompute::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

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

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

	{	/// Buffer

		terrainInfo_.Create(_dxm->GetDxDevice());
		inputInfo_.Create(_dxm->GetDxDevice());

	}
}

void TerrainVertexEditorCompute::Execute(class EntityComponentSystem* _ecs, DxCommand* _dxCommand, AssetCollection* _assetCollection) {

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
		return;
	}

	if (!pTerrain->GetIsCreated()) {
		return;
	}




	/// マウスが範囲外なら処理しない
	const Vector2& mousePosition = Input::GetImGuiImageMousePosNormalized("Scene");
	if (mousePosition.x < 0.0f || mousePosition.x > 1280.0f
		|| mousePosition.y < 0.0f || mousePosition.y > 720.0f) {
		return;
	}

	/// bufferに値をセット
	terrainInfo_.SetMappedData(TerrainInfo{ pTerrain->GetOwner()->GetId() });

	bool isRaiseTerrainButtonPressed = Input::TriggerMouse(Mouse::Left) && !Input::PressKey(DIK_LSHIFT);
	bool isLowerTerrainButtonPressed = Input::TriggerMouse(Mouse::Left) && Input::PressKey(DIK_LSHIFT);
	int byte = 0;
	byte |= (isRaiseTerrainButtonPressed << 0);
	byte |= (isLowerTerrainButtonPressed << 1);

	//if (Input::PressKey(DIK_LCONTROL) && !Input::PressKey(DIK_LSHIFT)) {

	//	/// 編集モードの変更
	//	if (Input::TriggerKey(DIK_N)) { editMode_ = 0; }
	//	if (Input::TriggerKey(DIK_V)) { editMode_ = 1; }
	//	if (Input::TriggerKey(DIK_B)) { editMode_ = 2; }

	//	/// 編集するテクスチャのインデックスの変更
	//	if (Input::TriggerKey(DIK_1)) { editTextureIndex_ = 0; }
	//	if (Input::TriggerKey(DIK_2)) { editTextureIndex_ = 1; }
	//	if (Input::TriggerKey(DIK_3)) { editTextureIndex_ = 2; }
	//	if (Input::TriggerKey(DIK_4)) { editTextureIndex_ = 3; }
	//}

	inputInfo_.SetMappedData(
		InputInfo{
			mousePosition,
			pTerrain->editorInfo_.brushRadius,
			pTerrain->editorInfo_.brushStrength,
			byte,
			pTerrain->editorInfo_.editMode,
			pTerrain->editorInfo_.usedTextureIndex
		}
	);



	/// 押していないときは処理をしない
	if (!Input::TriggerMouse(Mouse::Left)) {
		return;
	}



	/// pipelineの設定&実行
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto cmdList = _dxCommand->GetCommandList();

	/// CBV
	terrainInfo_.BindForComputeCommandList(cmdList, CBV_TERRAIN_INFO);
	inputInfo_.BindForComputeCommandList(cmdList, CBV_INPUT_INFO);

	/// UAV
	pTerrain->GetRwVertices().UAVBindForComputeCommandList(cmdList, UAV_VERTICES);

	/// SRV
	const Texture* positionTexture = _assetCollection->GetTexture("./Assets/Scene/RenderTexture/debugWorldPosition");
	const Texture* flagTexture = _assetCollection->GetTexture("./Assets/Scene/RenderTexture/debugFlags");

	cmdList->SetComputeRootDescriptorTable(SRV_POSITION_TEXTURE, positionTexture->GetSRVGPUHandle());
	cmdList->SetComputeRootDescriptorTable(SRV_FLAG_TEXTURE, flagTexture->GetSRVGPUHandle());

	const UINT threadGroupSize = 256;
	cmdList->Dispatch(
		(pTerrain->GetMaxVertexNum() + threadGroupSize - 1) / threadGroupSize,
		1, 1
	);
}
