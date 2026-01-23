#include "VoxelTerrainRenderingPipeline.h"

using namespace ONEngine;

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

namespace {
ConstantBuffer<Vector4> cBufPos;
}


VoxelTerrainRenderingPipeline::VoxelTerrainRenderingPipeline(AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
}

VoxelTerrainRenderingPipeline::~VoxelTerrainRenderingPipeline() {}


void VoxelTerrainRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	pDxManager_ = _dxm;

	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrainMarchingCube.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		//shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.ps.hlsl", L"ps_6_0", Shader::Type::ps);


		/// Pipeline
		pipeline_ = std::make_unique<GraphicsPipeline>();
		CreatePipeline(pipeline_.get(), shader, _dxm, D3D12_FILL_MODE_SOLID, BlendMode::Normal());
		wireframePipeline_ = std::make_unique<GraphicsPipeline>();
		CreatePipeline(wireframePipeline_.get(), shader, _dxm, D3D12_FILL_MODE_WIREFRAME, BlendMode::Multiply());
	}

	cBufPos.Create(_dxm->GetDxDevice());
	cBufPos.SetMappedData(Vector4(180.0f, 465.0f, 182.0f, 1.0f));
}

void VoxelTerrainRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// ---------------------------------------------------
	/// 早期リターンの条件チェック
	/// ---------------------------------------------------
	ComponentArray<VoxelTerrain>* voxelTerrainArray = _ecs->GetComponentArray<VoxelTerrain>();
	if(!voxelTerrainArray || voxelTerrainArray->GetUsedComponents().empty()) {
		return;
	}

	VoxelTerrain* voxelTerrain = nullptr;
	for(auto& vt : voxelTerrainArray->GetUsedComponents()) {
		if(CheckComponentEnable(vt)) {
			voxelTerrain = vt;
			break;
		}
	}

	if(!CheckComponentEnable(voxelTerrain)) {
		return;
	}

	auto cmdList = _dxCommand->GetCommandList();
	if(!voxelTerrain->CheckCreatedBuffers()) {
		voxelTerrain->SettingChunksGuid(pAssetCollection_);
		voxelTerrain->CreateBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap(), pAssetCollection_);
		return;
	}


	if(!voxelTerrain->canMeshShaderRendering_) {
		return;
	}

	/// ---------------------------------------------------
	/// 描画
	/// ---------------------------------------------------


	GPUTimeStamp::GetInstance().BeginTimeStamp(
		GPUTimeStampID::VoxelTerrainRegularCell
	);

	/// --------------- パイプラインの設定 --------------- ///
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	pDxManager_->HeapBindToCommandList();

	/// --------------- バッファの設定 --------------- ///
	voxelTerrain->SetupGraphicBuffers(cmdList, { CBV_VOXEL_TERRAIN_INFO, CBV_MATERIAL, SRV_CHUNK_ARRAY }, pAssetCollection_);

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_VIEW_PROJECTION);
	//_camera->GetCameraPosBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_CAMERA_POSITION);
	cBufPos.BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_CAMERA_POSITION);

	D3D12_GPU_DESCRIPTOR_HANDLE frontSRVHandle = pDxManager_->GetDxSRVHeap()->GetSRVStartGPUHandle();
	cmdList->SetGraphicsRootDescriptorTable(
		SRV_VOXEL_TERRAIN_TEXTURE3D, frontSRVHandle
	);
	//cmdList->SetGraphicsRootDescriptorTable(
	//	SRV_TEXTURES, frontSRVHandle
	//);


	/// --------------- ディスパッチ --------------- ///
	cmdList->DispatchMesh(
		voxelTerrain->GetChunkCountXZ().x,
		1,
		voxelTerrain->GetChunkCountXZ().y
	);


	GPUTimeStamp::GetInstance().EndTimeStamp(
		GPUTimeStampID::VoxelTerrainRegularCell
	);


	wireframePipeline_->SetPipelineStateForCommandList(_dxCommand);
	pDxManager_->HeapBindToCommandList();

	/// --------------- バッファの設定 --------------- ///
	voxelTerrain->SetupGraphicBuffers(cmdList, { CBV_VOXEL_TERRAIN_INFO, CBV_MATERIAL, SRV_CHUNK_ARRAY }, pAssetCollection_);

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_VIEW_PROJECTION);
	//_camera->GetCameraPosBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_CAMERA_POSITION);
	cBufPos.BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_CAMERA_POSITION);
	cmdList->SetGraphicsRootDescriptorTable(
		SRV_VOXEL_TERRAIN_TEXTURE3D, frontSRVHandle
	);
	//cmdList->SetGraphicsRootDescriptorTable(
	//	SRV_TEXTURES, frontSRVHandle
	//);

	/// --------------- ディスパッチ --------------- ///
	cmdList->DispatchMesh(
		voxelTerrain->GetChunkCountXZ().x,
		1,
		voxelTerrain->GetChunkCountXZ().y
	);

}

void ONEngine::VoxelTerrainRenderingPipeline::CreatePipeline(GraphicsPipeline* _pipeline, Shader& _shader, DxManager* _dxm, D3D12_FILL_MODE _fillMode, D3D12_BLEND_DESC _blendMode) {
	_pipeline->SetShader(&_shader);

	_pipeline->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // VoxelTerrainInfo
	_pipeline->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // ViewProjection
	_pipeline->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2); // CameraPosition
	_pipeline->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3); // Material

	_pipeline->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Chunk array
	_pipeline->AddDescriptorRange(1, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // VoxelTerrain Texture3D
	//_pipeline->AddDescriptorRange(3000, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Textures

	_pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // Chunk array
	_pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // VoxelTerrain Texture3D
	//_pipeline->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2); // Textures


	_pipeline->AddStaticSampler(StaticSampler::ClampSampler(), D3D12_SHADER_VISIBILITY_ALL, 0);
	//_pipeline->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 1);


	_pipeline->SetBlendDesc(_blendMode);
	_pipeline->SetFillMode(_fillMode);
	_pipeline->SetCullMode(D3D12_CULL_MODE_BACK);
	_pipeline->SetDepthStencilDesc(DefaultDepthStencilDesc());

	_pipeline->CreatePipeline(_dxm->GetDxDevice());
}
