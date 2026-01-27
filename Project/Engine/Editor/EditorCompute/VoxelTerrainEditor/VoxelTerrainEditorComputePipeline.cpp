#include "VoxelTerrainEditorComputePipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"

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
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_CHUNKS
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // SRV_WORLD_TEXTURE
		pipeline_->AddDescriptorRange(0, ONEngine::MAX_TEXTURE_COUNT * 2, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); // UAV_VOXEL_TEXTURES

		/// SRV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // SRV_CHUNKS
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // SRV_WORLD_TEXTURE

		/// UAV
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // UAV_VOXEL_TEXTURES

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

	{	/// member objects
		editCount_ = 0;
		maxEditCount_ = 100;
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

	/// UAV VoxelTextures
	cmdList->SetComputeRootDescriptorTable(
		UAV_VOXEL_TEXTURES, pDxManager_->GetDxSRVHeap()->GetSRVStartGPUHandle()
	);


	for(uint32_t chunkId = 0; chunkId < voxelTerrain->MaxChunkCount(); ++chunkId) {
		ONEngine::Cube chunkAABB;
		chunkAABB.center.x = (static_cast<float>(chunkId % voxelTerrain->GetChunkCountXZ().x)) * static_cast<float>(voxelTerrain->GetChunkSize().x);
		chunkAABB.center.y = static_cast<float>(voxelTerrain->GetChunkSize().y) * 0.5f;
		chunkAABB.center.z = (static_cast<float>(chunkId / voxelTerrain->GetChunkCountXZ().x)) * static_cast<float>(voxelTerrain->GetChunkSize().z);
		chunkAABB.size.x = static_cast<float>(voxelTerrain->GetChunkSize().x);
		chunkAABB.size.y = static_cast<float>(voxelTerrain->GetChunkSize().y);
		chunkAABB.size.z = static_cast<float>(voxelTerrain->GetChunkSize().z);

		if(!cameraComp->IsVisible(chunkAABB.center, chunkAABB.size)) {
			continue;
		}

		/// --------------- 32bit定数の設定 --------------- ///
		cmdList->SetComputeRoot32BitConstant(
			C32BIT_CHUNK_ID, chunkId, 0
		);
		/// --------------- ディスパッチ --------------- ///
		const uint32_t numthreads = 4;
		uint32_t brushSize = voxelTerrain->GetBrushRadius();
		cmdList->Dispatch(
			ONEngine::Math::DivideAndRoundUp(brushSize * 2, numthreads),
			ONEngine::Math::DivideAndRoundUp(brushSize * 2, numthreads),
			ONEngine::Math::DivideAndRoundUp(brushSize * 2, numthreads)
		);
	}

	/// 編集したのであればSRVに対してコピーを行う
	voxelTerrain->CopyEditorTextureToChunkTexture(_dxCommand);

	ONEngine::GPUTimeStamp::GetInstance().EndTimeStamp(
		ONEngine::GPUTimeStampID::VoxelTerrainEditorCompute
	);

}

void VoxelTerrainEditorComputePipeline::EditStart() {
	/// ---------------------------------------------------
	/// 編集を開始したときの状態を保存しておく
	/// ---------------------------------------------------



}

void VoxelTerrainEditorComputePipeline::EditEnd() {
	/// ---------------------------------------------------
	/// 編集を終了したときの状態を保存しておく
	/// ---------------------------------------------------



}