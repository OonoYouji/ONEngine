#include <Particle.h>

/// ===================================================
/// static member initialize
/// ===================================================
std::unique_ptr<PipelineState> Particle::sPipeline_ = nullptr;
PipelineState::Shader Particle::sShader_ = {};


/// ===================================================
/// 静的なメンバの初期化
/// ===================================================
void Particle::StaticInitialize() {

	sPipeline_.reset(new PipelineState);

	sShader_.ShaderCompile(
		L"Particle/Particle.VS.hlsl", L"vs_6_0",
		L"Particle/Particle.PS.hlsl", L"ps_6_0"
	);

	sPipeline_->SetFillMode(kSolid);

	sPipeline_->SetShader(&sShader_);
	sPipeline_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	sPipeline_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	sPipeline_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	sPipeline_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	
	sPipeline_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	/// viewProjection

	sPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	sPipeline_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0);	/// transform
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);	/// material
	sPipeline_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);	/// texture
	
	sPipeline_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);		/// sampler

	sPipeline_->Initialize();

}


/// ===================================================
/// 静的なメンバの初期化
/// ===================================================
void Particle::StaticFinalize() {
	sPipeline_.reset();
}
