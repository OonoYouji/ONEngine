#include "TerrainProceduralRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

TerrainProceduralRenderingPipeline::TerrainProceduralRenderingPipeline() {}
TerrainProceduralRenderingPipeline::~TerrainProceduralRenderingPipeline() {}

void TerrainProceduralRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// compute pipeline

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Editor/Procedural.cs.hlsl", L"cs_6_6", Shader::Type::cs);

		computePipeline_ = std::make_unique<ComputePipeline>();
		computePipeline_->SetShader(&shader);

		/// Buffer
		computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); /// UAV_INSTANCE_DATA
		computePipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV_VERTEX_TEXTURE

		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 0); /// UAV_INSTANCE_DATA
		computePipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_ALL, 1); /// SRV_VERTEX_TEXTURE

		computePipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0);

		computePipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}



	{
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"./Packages/Shader/Render/Terrain/TerrainProcedural.ps.hlsl", L"ps_6_6", Shader::Type::ps);
		shader.CompileShader(L"./Packages/Shader/Render/Terrain/TerrainProcedural.vs.hlsl", L"vs_6_6", Shader::Type::vs);


		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		/// InputLayout
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

		/// Buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// GP_CBV_VIEW_PROJECTION

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// GP_SRV_INSNTANCE_DATA
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// GP_SRV_INSNTANCE_DATA


		/// setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());
	}

}

void TerrainProceduralRenderingPipeline::Draw(EntityComponentSystem* _ecs, const std::vector<IEntity*>& _entities, CameraComponent* _camera, DxCommand* _dxCommand) {

}
