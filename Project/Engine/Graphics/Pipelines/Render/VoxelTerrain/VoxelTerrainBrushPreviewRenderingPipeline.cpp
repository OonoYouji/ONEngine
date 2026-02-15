#include "VoxelTerrainBrushPreviewRenderingPipeline.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/DirectX12/GPUTimeStamp/GPUTimeStamp.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/Graphics/Framework/RenderInfo.h"

namespace ONEngine {

VoxelTerrainBrushPreviewRenderingPipeline::VoxelTerrainBrushPreviewRenderingPipeline(AssetCollection* assetCollection)
	: pAssetCollection_(assetCollection) {
}
VoxelTerrainBrushPreviewRenderingPipeline::~VoxelTerrainBrushPreviewRenderingPipeline() = default;

void VoxelTerrainBrushPreviewRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {

	pDxm_ = _dxm;

	{	/// shader 
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/BrushPreview.as.hlsl", L"as_6_5", Shader::Type::as);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/BrushPreview.ms.hlsl", L"ms_6_5", Shader::Type::ms);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrain/BrushPreview.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// CBV
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 2);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 3);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 4);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_ALL, 5);

		/// SRV
		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorRange(2, MAX_TEXTURE_COUNT, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 2);

		pipeline_->AddStaticSampler(StaticSampler::ClampSampler(), D3D12_SHADER_VISIBILITY_ALL, 0);
		pipeline_->AddStaticSampler(StaticSampler::ClampSampler(), D3D12_SHADER_VISIBILITY_ALL, 1);

		auto blend = BlendMode::Normal();
		//blend.RenderTarget[0].RenderTargetWriteMask = 0;
		//blend.RenderTarget[1].RenderTargetWriteMask = 0;
		//blend.RenderTarget[2].RenderTargetWriteMask = 0;
		//blend.RenderTarget[3].RenderTargetWriteMask = 0;

		pipeline_->SetBlendDesc(blend);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}

	{	/// Buffer
		cBufferBrushInfo_.Create(_dxm->GetDxDevice());
		cBufferBrushInfo_.SetMappedData(BrushInfo{ Vector2(0.0f, 0.0f), 5, 0.5f });
	}
}

void VoxelTerrainBrushPreviewRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<VoxelTerrain>* voxelTerrains = _ecs->GetComponentArray<VoxelTerrain>();
	if(!CheckComponentArrayEnable(voxelTerrains)) {
		return;
	}

	VoxelTerrain* vt = nullptr;
	for(auto& comp : voxelTerrains->GetUsedComponents()) {
		if(CheckComponentEnable(comp)) {
			vt = comp;
			break;
		}
	}

	if(!CheckComponentEnable(vt)) {
		return;
	}

	if(!CheckVoxelTerrainBufferEnabled(vt)) {
		return;
	}

	if(!vt->isEditMode_) {
		return;
	}


	const Vector2& mousePos = Input::GetImGuiImageMousePosNormalized("Scene");
	const Vector2& mouseUV = mousePos / Vector2::HD;
	cBufferBrushInfo_.SetMappedData(
		BrushInfo{
			mouseUV,
			vt->GetBrushRadius(),
			vt->GetBrushStrength()
		}
	);

	if(mouseUV.x < 0.0f || mouseUV.x > 1.0f || mouseUV.y < 0.0f || mouseUV.y > 1.0f) {
		return;
	}



	/// --------------- パイプラインの設定 --------------- ///
	GPUTimeStamp::GetInstance().BeginTimeStamp(GPUTimeStampID::VoxelTerrainEditorBrushPreview);

	auto cmdList = _dxCommand->GetCommandList();
	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	pDxm_->HeapBindToCommandList();


	/// --------------- バッファの設定 --------------- ///

	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_VIEW_PROJECTION);
	_camera->GetCameraPosBuffer().BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_CAMERA_POSITION);
	vt->cBufferTerrainInfo_.BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_VOXEL_TERRAIN_INFO);
	vt->cBufferLODInfo_.BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_LOD_INFO);
	vt->cBufferMaterial_.BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_MATERIAL);
	vt->sBufferChunks_.SRVBindForGraphicsCommandList(_dxCommand->GetCommandList(), SRV_CHUNKS);
	cBufferBrushInfo_.BindForGraphicsCommandList(_dxCommand->GetCommandList(), CBV_BRUSH_INFO);


	/// WorldPositionTexture
	D3D12_GPU_DESCRIPTOR_HANDLE worldPosTexHandle = pAssetCollection_->GetTexture(RenderInfo::GetRenderTextureFullName(RenderInfo::RenderTexture::Debug, RenderInfo::RenderTextureType::WorldPosition))->GetSRVGPUHandle();
	cmdList->SetGraphicsRootDescriptorTable(
		SRV_WORLD_POSITION_TEXTURE, worldPosTexHandle
	);

	/// ALL Texture3D
	D3D12_GPU_DESCRIPTOR_HANDLE frontSRVHandle = pDxm_->GetDxSRVHeap()->GetSRVStartGPUHandle();
	cmdList->SetGraphicsRootDescriptorTable(
		SRV_VOXEL_TERRAIN_TEXTURE3D, frontSRVHandle
	);


	cmdList->DispatchMesh(
		vt->GetChunkCountXZ().x,
		1,
		vt->GetChunkCountXZ().y
	);


	GPUTimeStamp::GetInstance().EndTimeStamp(GPUTimeStampID::VoxelTerrainEditorBrushPreview);

}

bool VoxelTerrainBrushPreviewRenderingPipeline::CheckVoxelTerrainBufferEnabled(VoxelTerrain* vt) {
	if(!vt) return false;
	if(!vt->enable) return false;
	if(!vt->cBufferTerrainInfo_.Get()) return false;
	if(!vt->cBufferLODInfo_.Get()) return false;
	if(!vt->cBufferMaterial_.Get()) return false;
	if(!vt->sBufferChunks_.GetResource().Get()) return false;

	return true;
}


} /// namespace ONEngine