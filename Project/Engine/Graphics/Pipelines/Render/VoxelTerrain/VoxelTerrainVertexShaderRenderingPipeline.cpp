#include "VoxelTerrainVertexShaderRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"

using namespace ONEngine;

VoxelTerrainVertexShaderRenderingPipeline::VoxelTerrainVertexShaderRenderingPipeline(AssetCollection* _ac)
	: pAssetCollection_(_ac) {
}

VoxelTerrainVertexShaderRenderingPipeline::~VoxelTerrainVertexShaderRenderingPipeline() {}


void VoxelTerrainVertexShaderRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxm) {
	pDxManager_ = _dxm;

	{	/// Shader
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrainTest/VoxelTerrain.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"./Packages/Shader/Render/VoxelTerrainTest/VoxelTerrain.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); // CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1); // CBV_MATERIAL

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}
}

void VoxelTerrainVertexShaderRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {

	ComponentArray<VoxelTerrain>* voxelTerrainCompArray = _ecs->GetComponentArray<VoxelTerrain>();
	if(!CheckComponentArrayEnable(voxelTerrainCompArray)) {
		return;
	}

	VoxelTerrain* vt = nullptr;
	for(auto& voxelTerrain : voxelTerrainCompArray->GetUsedComponents()) {
		if(CheckComponentEnable(voxelTerrain)) {
			vt = voxelTerrain;
			break;
		}
	}

	if(!vt || !CheckComponentEnable(vt)) {
		return;
	}

	if(!vt->CheckCreatedBuffers()) {
		vt->SettingChunksGuid(pAssetCollection_);
		vt->CreateBuffers(pDxManager_->GetDxDevice(), pDxManager_->GetDxSRVHeap(), pAssetCollection_);
		return;
	}


	vt->SettingMaterial();

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	auto cmdList = _dxCommand->GetCommandList();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_camera->GetViewProjectionBuffer().BindForGraphicsCommandList(cmdList, CBV_VIEW_PROJECTION);
	vt->cBufferMaterial_.BindForGraphicsCommandList(cmdList, CBV_MATERIAL);

	for(size_t i = 0; i < 3; i++) {
		//for(size_t i = 0; i < vt->chunks_.size(); i++) {
		auto& chunk = vt->chunks_[i];
		//if(!chunk.rwVertices.GetResource().Get() || chunk.vertexCount == 0) {
		//	continue;
		//}

		chunk.vbv.BindForCommandList(cmdList);
		cmdList->DrawInstanced(80000, 1, 0, 0);
	}

}