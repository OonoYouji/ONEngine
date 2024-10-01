#include "ParticleSystem.h"


std::unique_ptr<ParticlePipeline> ParticleSystem::sPipeline_ = nullptr;




/// ===================================================
/// ParticleSystem 
/// ===================================================

ParticleSystem::ParticleSystem(uint32_t maxParticleNum) : kMaxParticleNum_(maxParticleNum) {

}


void ParticleSystem::SInitialize() {
	if(!sPipeline_) {
		sPipeline_.reset(new ParticlePipeline);
		sPipeline_->Initialize();
	}
}

void ParticleSystem::SFinalize() {
	sPipeline_.reset();
}


void ParticleSystem::Initialize() {
	particleArray_.reserve(kMaxParticleNum_);
}



void ParticleSystem::Update() {}

void ParticleSystem::Draw() {}




/// ===================================================
/// ParticlePipeline 
/// ===================================================

void ParticlePipeline::Initialize() {

	shader_.ShaderCompile(
		L"Particle/Particle.VS.hlsl", L"vs_6_0",
		L"Particle/Particle.PS.hlsl", L"ps_6_0"
	);

	pipelineState_.reset(new PipelineState);

	/// Shader, FillMode
	pipelineState_->SetShader(&shader_);
	pipelineState_->SetFillMode(kSolid);

	/// Topology
	pipelineState_->SetTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	/// Input Layout
	pipelineState_->AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	pipelineState_->AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	pipelineState_->AddInputElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);

	/// Constant Buffer
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- transform
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL,  0);	///- material
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL,  1);	///- directional light

	/// SRV - Texture
	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL,   0);

	/// Pipeline Create
	pipelineState_->Initialize();

}
