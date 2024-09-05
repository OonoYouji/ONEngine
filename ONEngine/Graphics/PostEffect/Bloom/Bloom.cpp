#include "Bloom.h"

#include <RenderTextureManager.h>


std::unique_ptr<PipelineState> Bloom::sPipeline_ = nullptr;


void Bloom::StaticInitialize() {
	PipelineState::Shader shader{};
	shader.ShaderCompile(
		L"Bloom/Bloom.VS.hlsl", L"vs_6_0",
		L"Bloom/Bloom.PS.hlsl", L"ps_6_0"
	);

	sPipeline_->SetShader(&shader);
	sPipeline_->SetFillMode(kSolid);
	sPipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sPipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sPipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	/// vs cbuffer
	sPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// ps cbuffer
	sPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	sPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	sPipeline_->Initialize();

	RenderTextureManager::CreateRenderTarget("bloom", 3);
	RenderTextureManager::SetIsBlending("bloom", false);

}

void Bloom::StaticFinalize() {
	sPipeline_.reset();
}



std::unique_ptr<RenderTexture> Bloom::CreateBroomRenderTexture(RenderTexture* sourceRenderTexture) {
	auto renderTex = RenderTextureManager::GetRenderTarget("bloom");
	
	return nullptr;
}
