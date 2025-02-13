#include "Line3DRenderingPipeline.h"

Line3DRenderingPipeline::Line3DRenderingPipeline() {}
Line3DRenderingPipeline::~Line3DRenderingPipeline() {}

void Line3DRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxDevice* _dxDevice) {
	{	/// pipelineの作成
		
		/// shaderをコンパイル
		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Assets/Shader/Line/Line3D.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Assets/Shader/Line/Line3D.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_.reset(new GraphicsPipeline());
		pipeline_->SetShader(&shader);

		/// input element setting
		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		/// rasterizer desc setting
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		/// buffer
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); ///< transform
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1); ///< view projection

		/// topology type setting
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

		/// blend desc setting
		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable           = TRUE;
		blendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		pipeline_->SetBlendDesc(blendDesc);

		/// depth stencil desc setting
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable    = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc      = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		/// create pipeline
		pipeline_->CreatePipeline(_dxDevice);
	}

}

void Line3DRenderingPipeline::Draw([[maybe_unused]] DxCommand* _dxCommand, [[maybe_unused]] EntityCollection* _entityCollection) {
}
