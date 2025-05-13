#include "SkyboxRenderingPipeline.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

SkyboxRenderingPipeline::SkyboxRenderingPipeline() {}
SkyboxRenderingPipeline::~SkyboxRenderingPipeline() {}

void SkyboxRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{	/// shader

		Shader shader;
		shader.Initialize(_shaderCompiler);
		shader.CompileShader(L"Packages/Shader/Render/Skybox/Skybox.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/Skybox/Skybox.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);


		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);
		pipeline_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);



		pipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		//pipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT);

		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);

		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		pipeline_->SetBlendDesc(BlendMode::Normal());

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->SetRTVNum(4); /// 色、ワールド座標、法線、フラグ
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, 2);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 3);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

}

void SkyboxRenderingPipeline::Draw(DxCommand* _dxCommand, EntityComponentSystem* _pEntityComponentSystem, Camera* _camera) {}
