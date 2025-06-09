#include "CopyImageRenderingPipeline.h"

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"


CopyImageRenderingPipeline::CopyImageRenderingPipeline(GraphicsResourceCollection* _graphicsResourceCollection)
	: pResourceCollection_(_graphicsResourceCollection) {}

void CopyImageRenderingPipeline::Initialize(ShaderCompiler* _shaderCompiler, DxManager* _dxManager) {

	{
		Shader shader;
		shader.Initialize(_shaderCompiler);

		shader.CompileShader(L"Packages/Shader/Render/CopyImage/CopyImage.vs.hlsl", L"vs_6_0", Shader::Type::vs);
		shader.CompileShader(L"Packages/Shader/Render/CopyImage/CopyImage.ps.hlsl", L"ps_6_0", Shader::Type::ps);

		pipeline_ = std::make_unique<GraphicsPipeline>();
		pipeline_->SetShader(&shader);

		pipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); ///< texture
		pipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);      ///< texture
		pipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);        ///< texture sampler

		pipeline_->SetCullMode(D3D12_CULL_MODE_NONE);
		pipeline_->SetFillMode(D3D12_FILL_MODE_SOLID);
		pipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_->SetBlendDesc(BlendMode::Normal());

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipeline_->SetDepthStencilDesc(depthStencilDesc);

		pipeline_->SetRTVNum(1);
		pipeline_->SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		pipeline_->CreatePipeline(_dxManager->GetDxDevice());

	}

}

void CopyImageRenderingPipeline::Draw(DxCommand* _dxCommand, [[maybe_unused]] EntityComponentSystem* _pEntityComponentSystem, [[maybe_unused]] Camera* _camera) {

	pipeline_->SetPipelineStateForCommandList(_dxCommand);
	ID3D12GraphicsCommandList* commandList = _dxCommand->GetCommandList();

	/// settings
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto& textures = pResourceCollection_->GetTextures();
	size_t index = pResourceCollection_->GetTextureIndex("scene");

	commandList->SetGraphicsRootDescriptorTable(0, textures[index]->GetSRVGPUHandle());

	commandList->DrawInstanced(3, 1, 0, 0);

}
