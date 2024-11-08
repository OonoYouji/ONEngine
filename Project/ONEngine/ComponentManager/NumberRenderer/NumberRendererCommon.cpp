#include "NumberRendererCommon.h"

void NumberRendererCommon::Initialize() {

	/// shader compile
	shader_.ShaderCompile(
		L"NumberRender/NumberRender.VS.hlsl", L"vs_6_0",
		L"NumberRender/NumberRender.PS.hlsl", L"ps_6_0"
	);

	pipelineState_.reset(new PipelineState);

	/// default setting
	pipelineState_->SetShader(&shader_);
	pipelineState_->SetFillMode(kSolid);
	pipelineState_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


	/// input layout setting
	pipelineState_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineState_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);


	/// buffer setting
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// view projection

	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0); /// transform 
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);  /// texture
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	pipelineState_->Initialize();

}

void NumberRendererCommon::Finalize() {
	pipelineState_.reset();
}



void NumberRendererCommon::PreDraw() {

}

void NumberRendererCommon::PostDraw() {

	pipelineState_->SetPipelineState();

}
