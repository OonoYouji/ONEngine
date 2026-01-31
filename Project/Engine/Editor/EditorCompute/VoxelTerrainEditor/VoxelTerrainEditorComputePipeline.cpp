#include "VoxelTerrainEditorComputePipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/CollisionCheck/CollisionCheck.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/Graphics/Framework/RenderInfo.h"

using namespace Editor;

VoxelTerrainEditorComputePipeline::VoxelTerrainEditorComputePipeline() = default;
VoxelTerrainEditorComputePipeline::~VoxelTerrainEditorComputePipeline() = default;

void VoxelTerrainEditorComputePipeline::Initialize(ONEngine::ShaderCompiler* _shaderCompiler, ONEngine::DxManager* _dxm) {

	pDxManager_ = _dxm;

	{	/// Shader
		ONEngine::Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/VoxelTerrainEditor.cs.hlsl", L"cs_6_6", ONEngine::Shader::Type::cs);

		pipeline_ = std::make_unique<ONEngine::ComputePipeline>();
		pipeline_->SetShader(&shader);

		/// CBV
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // CBV_TERRAIN_INFO
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2); // CBV_CAMERA
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3); // CBV_INPUT_INFO
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 4); // CBV_EDITOR_INFO

		pipeline_->Add32BitConstant(D3D12_SHADER_VISIBILITY_ALL, 5, 1); // C32BIT_CHUNK_ID

		/// Descriptor Range
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_MOUSE_POS_BUFFER
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CHUNKS
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_WORLD_TEXTURE
		pipeline_->AddDescriptorRange(1, ONEngine::MAX_TEXTURE_COUNT * 2, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_VOXEL_TEXTURES

		/// SRV, UAV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // UAV_VOXEL_TEXTURES
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV_CHUNKS
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // SRV_WORLD_TEXTURE
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 3); // UAV_MOUSE_POS_BUFFER

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

	{
		ONEngine::Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/CalculationMouseWorldPosition.cs.hlsl", L"cs_6_6", ONEngine::Shader::Type::cs);

		calculationMouseWorldPosPipeline_ = std::make_unique<ONEngine::ComputePipeline>();
		calculationMouseWorldPosPipeline_->SetShader(&shader);

		calculationMouseWorldPosPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // CBV_INPUT_INFO

		calculationMouseWorldPosPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_MOUSE_POS_BUFFER
		calculationMouseWorldPosPipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // UAV_MOUSE_POS_BUFFER

		calculationMouseWorldPosPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // UAV_MOUSE_POS_BUFFER
		calculationMouseWorldPosPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV_WORLD_TEXTURE

		calculationMouseWorldPosPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		calculationMouseWorldPosPipeline_->CreatePipeline(_dxm->GetDxDevice());

	}


	{	/// member objects
		uavMousePosBuffer_.CreateUAV(
			1,
			pDxManager_->GetDxDevice(),
			pDxManager_->GetDxCommand(),
			pDxManager_->GetDxSRVHeap()
		);

		cBufferMouseUV_.Create(pDxManager_->GetDxDevice());
	}

}

void VoxelTerrainEditorComputePipeline::Execute(ONEngine::EntityComponentSystem* _ecs, ONEngine::DxCommand* _dxCommand, ONEngine::AssetCollection* _assetCollection) {

	/// 早期リターンの条件チェック
	ONEngine::ComponentArray<ONEngine::VoxelTerrain>* voxelTerrainArray = _ecs->GetCurrentGroup()->GetComponentArray<ONEngine::VoxelTerrain>();
	if(!voxelTerrainArray || voxelTerrainArray->GetUsedComponents().empty()) {
		ONEngine::Console::LogWarning("VoxelTerrainEditorComputePipeline::Execute: VoxelTerrain component array is null");
		return;
	}


	/// 使用できるVoxelTerrainコンポーネントを探す
	ONEngine::VoxelTerrain* voxelTerrain = nullptr;
	for(const auto& vt : voxelTerrainArray->GetUsedComponents()) {
		if(vt && vt->enable) {
			voxelTerrain = vt;
			break;
		}
	}

	/// 見つからなかった
	if(!voxelTerrain) {
		return;
	}


	pDxManager_->HeapBindToCommandList();
	/// --------------- バッファの生成 --------------- ///
	if(!voxelTerrain->CheckCreatedBuffers()) {
		voxelTerrain->SettingChunksGuid(_assetCollection);
		voxelTerrain->CreateBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap(), _assetCollection);
	}

	if(!voxelTerrain->CheckBufferCreatedForEditor()) {
		voxelTerrain->CreateEditorBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap());
		voxelTerrain->CreateChunkTextureUAV(_dxCommand, pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap(), _assetCollection);
		return;
	}


	if(!voxelTerrain->CanMeshShaderRendering()) {
		return;
	}


	/// ---------------------------------------------------
	/// ここから パイプラインの設定、実行
	/// ---------------------------------------------------
	ONEngine::GPUTimeStamp::GetInstance().BeginTimeStamp(
		ONEngine::GPUTimeStampID::VoxelTerrainEditorCompute
	);

	ExecuteCalculateMouseWorldPos(_dxCommand, _assetCollection);

	pipeline_->SetPipelineStateForCommandList(_dxCommand);

	auto cmdList = _dxCommand->GetCommandList();

	ONEngine::GPUData::InputInfo inputInfo{};
	inputInfo.mouseLeftButton = ONEngine::Input::PressMouse(ONEngine::Mouse::Left);
	inputInfo.keyboardKShift = ONEngine::Input::PressKey(DIK_LSHIFT);
	inputInfo.screenMousePos = ONEngine::Input::GetImGuiImageMousePosNormalized("Scene");

	/// マウスがウィンドウ外なら終了
	if(!ONEngine::Math::Inside(inputInfo.screenMousePos, ONEngine::Vector2::Zero, ONEngine::Vector2::HD)) {
		return;
	}

	if(!voxelTerrain->IsEditMode()) {
		return;
	}

	voxelTerrain->SetupEditorBuffers(
		cmdList,
		{ CBV_INPUT_INFO, CBV_TERRAIN_INFO, CBV_EDITOR_INFO, SRV_CHUNKS },
		_assetCollection, inputInfo
	);


	ONEngine::CameraComponent* cameraComp = _ecs->GetECSGroup("Debug")->GetMainCamera();
	/// cameraBufferが生成済みでないなら終了
	if(!cameraComp->IsMakeViewProjection()) {
		ONEngine::Console::LogWarning("VoxelTerrainEditorComputePipeline::Execute: Camera viewProjection buffer is not created");
		return;
	}

	cameraComp->GetViewProjectionBuffer().BindForComputeCommandList(cmdList, CBV_VIEW_PROJECTION);
	cameraComp->GetCameraPosBuffer().BindForComputeCommandList(cmdList, CBV_CAMERA);

	/// WorldTexture
	const ONEngine::Texture* worldTexture = _assetCollection->GetTexture("./Assets/Scene/RenderTexture/debugWorldPosition");
	cmdList->SetComputeRootDescriptorTable(
		SRV_WORLD_TEXTURE,
		worldTexture->GetSRVHandle().gpuHandle
	);


	uavMousePosBuffer_.UAVBindForComputeCommandList(
		cmdList, UAV_MOUSE_POS
	);

	/// UAV VoxelTextures
	cmdList->SetComputeRootDescriptorTable(
		UAV_VOXEL_TEXTURES, pDxManager_->GetDxSRVHeap()->GetSRVStartGPUHandle()
	);

	/// mousePosを中心に3x3のチャンクを編集する
	const ONEngine::Vector3Int kNumthreads = { 10, 10, 10 };
	cmdList->Dispatch(
		ONEngine::Math::DivideAndRoundUp(voxelTerrain->GetBrushRadius() * 2, kNumthreads.x),
		ONEngine::Math::DivideAndRoundUp(voxelTerrain->GetBrushRadius() * 2, kNumthreads.y),
		ONEngine::Math::DivideAndRoundUp(voxelTerrain->GetBrushRadius() * 2, kNumthreads.z)
	);

	mouseWorldPos_ = uavMousePosBuffer_.Readback(_dxCommand, 0);
	std::vector<int> editedChunkIDs = GetEditedChunkIDs(voxelTerrain);

	/// 編集したのであればSRVに対してコピーを行う
	voxelTerrain->CopyEditorTextureToChunkTexture(_dxCommand, editedChunkIDs);

	ONEngine::GPUTimeStamp::GetInstance().EndTimeStamp(
		ONEngine::GPUTimeStampID::VoxelTerrainEditorCompute
	);

}

void VoxelTerrainEditorComputePipeline::ExecuteCalculateMouseWorldPos(ONEngine::DxCommand* dxCommand, ONEngine::AssetCollection* assetCollection) {
	ONEngine::Vector2 mouseUV = ONEngine::Input::GetImGuiImageMousePosNormalized("Scene");
	mouseUV /= ONEngine::Vector2::HD;

	/// マウスがウィンドウ外なら終了
	if(!ONEngine::Math::Inside(mouseUV, ONEngine::Vector2::Zero, ONEngine::Vector2::One)) {
		return;
	}


	calculationMouseWorldPosPipeline_->SetPipelineStateForCommandList(dxCommand);
	auto cmdList = dxCommand->GetCommandList();


	cBufferMouseUV_.SetMappedData(mouseUV);
	cBufferMouseUV_.BindForComputeCommandList(
		cmdList, C_MOUSE_POS_CBV_INPUT_INFO
	);

	uavMousePosBuffer_.UAVBindForComputeCommandList(
		cmdList, C_MOUSE_POS_UAV_MOUSE_POS
	);

	const std::string worldTexturePath = ONEngine::RenderInfo::GetRenderTextureFullName(ONEngine::RenderInfo::RenderTexture::Debug, ONEngine::RenderInfo::RenderTextureType::WorldPosition);
	D3D12_GPU_DESCRIPTOR_HANDLE handle = assetCollection->GetTexture(worldTexturePath)->GetSRVGPUHandle();
	cmdList->SetComputeRootDescriptorTable(C_MOUSE_POS_SRV_WORLD_TEXTURE, handle);

	cmdList->Dispatch(1, 1, 1);

}

std::vector<int> Editor::VoxelTerrainEditorComputePipeline::GetEditedChunkIDs(ONEngine::VoxelTerrain* vt) {
	std::vector<int> result(9);

	ONEngine::Vector3 terrainOrigin = vt->GetOwner()->GetPosition();
	ONEngine::Vector2 terrainLocal = ONEngine::Vector2(
		mouseWorldPos_.x - terrainOrigin.x,
		mouseWorldPos_.z - terrainOrigin.z
	);

	ONEngine::Vector2Int chunkIndex = ONEngine::Vector2Int(
		static_cast<int>(terrainLocal.x) / vt->GetChunkSize().x,
		static_cast<int>(terrainLocal.y) / vt->GetChunkSize().z
	);

	int chunkID = chunkIndex.y * vt->GetChunkCountXZ().x + chunkIndex.x;

	/// 中心のチャンクID
	result[0] = chunkID;

	/// 周囲8チャンクID
	result[1] = chunkID - 1; // 左
	result[2] = chunkID + 1; // 右
	result[3] = chunkID + vt->GetChunkCountXZ().x; // 上
	result[4] = chunkID - vt->GetChunkCountXZ().x; // 下
	result[5] = chunkID + vt->GetChunkCountXZ().x - 1; // 左上
	result[6] = chunkID + vt->GetChunkCountXZ().x + 1; // 右上
	result[7] = chunkID - vt->GetChunkCountXZ().x - 1; // 左下
	result[8] = chunkID - vt->GetChunkCountXZ().x + 1; // 右下

	for(auto& id : result) {
		if(id < 0 || id >= static_cast<int>(vt->MaxChunkCount())) {
			id = -1; // 無効なIDとして扱う
		}
	}

	return result;
}
