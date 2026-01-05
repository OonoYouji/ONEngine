#include "VoxelTerrainVertexShaderRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"


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

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); // CBV_VIEW_PROJECTION
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1); // CBV_MATERIAL

		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetDepthStencilDesc(DefaultDepthStencilDesc());
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->CreatePipeline(_dxm->GetDxDevice());
	}
}

void VoxelTerrainVertexShaderRenderingPipeline::Draw(ECSGroup* _ecs, CameraComponent* _camera, DxCommand* _dxCommand) {
}