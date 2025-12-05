#include "VoxelTerrainEditorComputePipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"

VoxelTerrainEditorComputePipeline::VoxelTerrainEditorComputePipeline() = default;
VoxelTerrainEditorComputePipeline::~VoxelTerrainEditorComputePipeline() = default;

void VoxelTerrainEditorComputePipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	pDxManager_ = _dxm;

	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/VoxelTerrainEditor.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		pipeline_ = std::make_unique<ComputePipeline>();
		pipeline_->SetShader(&shader);

		/// CBV
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // CBV_TERRAIN_INFO
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2); // CBV_CAMERA
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3); // CBV_INPUT_INFO
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 4); // CBV_EDITOR_INFO

		/// Descriptor Range
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CHUNKS
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_WORLD_TEXTURE
		pipeline_->AddDescriptorRange(0, MAX_TEXTURE_COUNT*2, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_VOXEL_TEXTURES

		/// SRV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // SRV_CHUNKS
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV_WORLD_TEXTURE

		/// UAV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // UAV_VOXEL_TEXTURES

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void VoxelTerrainEditorComputePipeline::Execute(EntityComponentSystem* _ecs, DxCommand* _dxCommand, AssetCollection* _assetCollection) {

	/// 早期リターンの条件チェック
	ComponentArray<VoxelTerrain>* voxelTerrainArray = _ecs->GetCurrentGroup()->GetComponentArray<VoxelTerrain>();
	if (!voxelTerrainArray || voxelTerrainArray->GetUsedComponents().empty()) {
		Console::LogWarning("VoxelTerrainEditorComputePipeline::Execute: VoxelTerrain component array is null");
		return;
	}


	/// 使用できるVoxelTerrainコンポーネントを探す
	VoxelTerrain* voxelTerrain = nullptr;
	for (const auto& vt : voxelTerrainArray->GetUsedComponents()) {
		if (vt && vt->enable) {
			voxelTerrain = vt;
			break;
		}
	}

	/// 見つからなかった
	if (!voxelTerrain) {
		return;
	}


	pDxManager_->HeapBindToCommandList();
	/// --------------- バッファの生成 --------------- ///
	if (!voxelTerrain->CheckCreatedBuffers()) {
		voxelTerrain->SettingChunksGuid(_assetCollection);
		voxelTerrain->CreateBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap());
	}

	if (!voxelTerrain->CheckBufferCreatedForEditor()) {
		voxelTerrain->CreateEditorBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap());
		voxelTerrain->CreateChunkTextureUAV(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap(), _assetCollection);
		voxelTerrain->TransitionTextureStates(
			_dxCommand,
			_assetCollection,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		return;
	}



	/// ---------------------------------------------------
	/// ここから パイプラインの設定、実行
	/// ---------------------------------------------------

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto cmdList = _dxCommand->GetCommandList();

	GPUData::InputInfo inputInfo{};
	inputInfo.mouseLeftButton = Input::PressMouse(Mouse::Left);
	inputInfo.keyboardKShift = Input::PressKey(DIK_LSHIFT);
	inputInfo.screenMousePos = Input::GetImGuiImageMousePosNormalized("Scene");

	GPUData::EditInfo editInfo{};
	editInfo.brushRadius = 10.0f;

	voxelTerrain->SetupEditorBuffers(
		cmdList,
		{ CBV_INPUT_INFO, CBV_TERRAIN_INFO, CBV_EDITOR_INFO, SRV_CHUNKS },
		_assetCollection, inputInfo, editInfo
	);


	CameraComponent* cameraComp = _ecs->GetCurrentGroup()->GetMainCamera();
	/// cameraBufferが生成済みでないなら終了
	if (!cameraComp->IsMakeViewProjection()) {
		Console::LogWarning("VoxelTerrainEditorComputePipeline::Execute: Camera viewProjection buffer is not created");
		return;
	}

	cameraComp->GetViewProjectionBuffer().BindForComputeCommandList(cmdList, CBV_VIEW_PROJECTION);
	cameraComp->GetCameraPosBuffer().BindForComputeCommandList(cmdList, CBV_CAMERA);

	/// WorldTexture
	const Texture* worldTexture = _assetCollection->GetTexture("./Assets/Scene/RenderTexture/debugWorldPosition");
	cmdList->SetComputeRootDescriptorTable(
		SRV_WORLD_TEXTURE,
		worldTexture->GetSRVHandle().gpuHandle
	);

	/// UAV VoxelTextures
	cmdList->SetComputeRootDescriptorTable(
		UAV_VOXEL_TEXTURES, pDxManager_->GetDxSRVHeap()->GetSRVStartGPUHandle()
	);


	const UINT TGSize = 256;
	const Vector2Int& voxelChunkCount = voxelTerrain->GetChunkCountXZ();
	cmdList->Dispatch(
		Mathf::DivideAndRoundUp(voxelChunkCount.x * voxelChunkCount.y, TGSize),
		1, 1
	);
}
