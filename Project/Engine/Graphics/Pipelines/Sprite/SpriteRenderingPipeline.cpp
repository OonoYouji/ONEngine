#include "SpriteRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"



SpriteRenderingPipeline::SpriteRenderingPipeline(GraphicsResourceCollection* _resourceCollection)
	: resourceCollection_(_resourceCollection) {}
SpriteRenderingPipeline::~SpriteRenderingPipeline() {}


void SpriteRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// pipeline 

		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Assets/Shader/Sprite/Sprite.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Assets/Shader/Sprite/Sprite.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_BACK);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->AddDescriptorRange(0, 32, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);      ///< texture : 1

		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);        ///< texture sampler

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

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable                    = TRUE;
		depthStencilDesc.DepthWriteMask                 = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc                      = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

}

void SpriteRenderingPipeline::Draw(DxCommand* _dxCommand, EntityCollection* _entityCollection) {

	_dxCommand;
	_entityCollection;

}
