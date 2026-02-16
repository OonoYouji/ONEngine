#include "VoxelTerrainTransvoxelRenderingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


using namespace ONEngine;

namespace {
ConstantBuffer<Vector4> cBufPos;
}



VoxelTerrainTransvoxelRenderingPipeline::VoxelTerrainTransvoxelRenderingPipeline(AssetCollection* _ac)
	: pAssetCollection_(_ac) {
}

VoxelTerrainTransvoxelRenderingPipeline::~VoxelTerrainTransvoxelRenderingPipeline() = default;

void VoxelTerrainTransvoxelRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	pDxManager_ = _dxm;

	{	/// shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/Transvoxel.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/Transvoxel.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/Transvoxel.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // VoxelTerrainInfo
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // ViewProjection
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2); // CameraPosition
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3); // LodInfo
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 4); // Material

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Chunk array
		pipeline_->AddDescriptorRange(1, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // VoxelTerrain Texture3D

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // Chunk array
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // VoxelTerrain Texture3D


		pipeline_->AddStaticSampler(StaticSampler::ClampSampler(), D3D12_SHADER_VISIBILITY_ALL, 0);

		pipeline_->SetBlendDesc(BlendMode::None());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetFillMode(D3D12_FILL_MODE_WIREFRAME);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->CreatePipeline(_dxm->GetDxDevice());

	}

	cBufPos.Create(_dxm->GetDxDevice());
	cBufPos.SetMappedData(Vector4(180.0f, 465.0f, 182.0f, 1.0f));

}

void VoxelTerrainTransvoxelRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<VoxelTerrain>* voxelTerrainArray = _ecs->GetComponentArray<VoxelTerrain>();
	if(!CheckComponentArrayEnable(voxelTerrainArray)) {
		return;
	}

	VoxelTerrain* vt = nullptr;
	for(auto& comp : voxelTerrainArray->GetUsedComponents()) {
		if(CheckComponentEnable(comp)) {
			vt = comp;
			break;
		}
	}

	if(!CheckComponentEnable(vt)) {
		return;
	}



	auto cmdList = _dxCommand->GetCommandList();
	if(!vt->CheckCreatedBuffers()) {
		vt->SettingChunksGuid(pAssetCollection_);
		vt->CreateBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap(), pAssetCollection_);
		return;
	}

	/// ---------------------------------------------------
	/// 描画
	/// ---------------------------------------------------

	GPUTimeStamp::GetInstance().BeginTimeStamp(
		GPUTimeStampID::VoxelTerrainTransitionCell
	);

	/// --------------- パイプラインの設定 --------------- ///
	if(vt->isRenderingTransvoxel_) {

		pipeline_->SetPipelineStateForCommandList(_dxCommand);
		pDxManager_->HeapBindToCommandList();

		/// --------------- バッファの設定 --------------- ///
		vt->SetupGraphicBuffers(cmdList, { CBV_VOXEL_TERRAIN_INFO, CBV_MATERIAL, SRV_CHUNK_ARRAY, CBV_LOD_INFO }, pAssetCollection_);

		_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_VIEW_PROJECTION);
		//_camera->GetCameraPosBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_CAMERA_POSITION);
		cBufPos.BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_CAMERA_POSITION);

		D3D12_GPU_DESCRIPTOR_HANDLE frontSRVHandle = pDxManager_->GetDxSRVHeap()->GetSRVStartGPUHandle();
		cmdList->SetGraphicsRootDescriptorTable(
			SRV_VOXEL_TERRAIN_TEXTURE3D, frontSRVHandle
		);

		/// --------------- ディスパッチ --------------- ///
		cmdList->DispatchMesh(
			vt->chunkCountXZ_.x,
			1,
			vt->chunkCountXZ_.y
		);

		GPUTimeStamp::GetInstance().EndTimeStamp(
			GPUTimeStampID::VoxelTerrainTransitionCell
		);

	}

}