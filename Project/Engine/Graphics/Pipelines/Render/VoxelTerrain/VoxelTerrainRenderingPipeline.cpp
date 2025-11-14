#include "VoxelTerrainRenderingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"


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
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/VoxelTerrain.ps.hlsl", L"ps_6_0", Shader::Type::ps);


		/// Pipeline
		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0); // VoxelTerrainInfo
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1); // ViewProjection

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Chunk array
		pipeline_->AddDescriptorRange(1, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // VoxelTerrain Texture3D

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); // Chunk array
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); // VoxelTerrain Texture3D


		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);


		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

}

void VoxelTerrainRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	/// ---------------------------------------------------
	/// 早期リターンの条件チェック
	/// ---------------------------------------------------
	ComponentArray<VoxelTerrain>* voxelTerrainArray = _ecs->GetComponentArray<VoxelTerrain>();
	if (!voxelTerrainArray || voxelTerrainArray->GetUsedComponents().empty()) {
		return;
	}

	VoxelTerrain* voxelTerrain = nullptr;
	for (auto& vt : voxelTerrainArray->GetUsedComponents()) {
		if (vt->enable) {
			voxelTerrain = vt;
			break;
		}
	}

	if (!voxelTerrain) {
		return;
	}


	/// ---------------------------------------------------
	/// 描画
	/// ---------------------------------------------------

	/// --------------- パイプラインの設定 --------------- ///
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	pDxManager_->HeapBindToCommandList();


	/// --------------- バッファの設定 --------------- ///

	auto cmdList = _dxCommand->GetCommandList();

	if (!voxelTerrain->CheckCreatedBuffers()) {
		voxelTerrain->SettingChunksGuid(pAssetCollection_);
		voxelTerrain->CreateBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap());
	}

	voxelTerrain->SetupGraphicBuffers(cmdList, { CBV_VOXEL_TERRAIN_INFO, SRV_CHUNK_ARRAY }, pAssetCollection_);
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_VIEW_PROJECTION);

	const Texture& frontTexture = pAssetCollection_->GetTextures().front();
	cmdList->SetGraphicsRootDescriptorTable(
		SRV_VOXEL_TERRAIN_TEXTURE3D, frontTexture.GetSRVHandle().gpuHandle
	);


	/// --------------- ディスパッチ --------------- ///
	cmdList->DispatchMesh(
		/*voxelTerrain->MaxChunkCount(),*/
		1, 1, 1
	);


}
