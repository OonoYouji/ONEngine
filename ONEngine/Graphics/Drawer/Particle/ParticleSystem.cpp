#include "ParticleSystem.h"

#include <cassert>
#include <algorithm>


std::unique_ptr<ParticlePipeline> ParticleSystem::sPipeline_ = nullptr;




/// ===================================================
/// ParticleSystem 
/// ===================================================

ParticleSystem::ParticleSystem(uint32_t maxParticleNum) : kMaxParticleNum_(maxParticleNum) {

}


void ParticleSystem::SInitialize(ID3D12GraphicsCommandList* commandList) {
	if(!sPipeline_) {
		sPipeline_.reset(new ParticlePipeline);
		sPipeline_->Initialize(commandList);
	}
}

void ParticleSystem::SFinalize() {
	sPipeline_.reset();
}


void ParticleSystem::Initialize() {
	particleArray_.reserve(kMaxParticleNum_);
	for(uint32_t i = 0u; i < 2; ++i) {
		particleArray_.push_back(std::make_unique<Particle>());
	}

}



void ParticleSystem::Update() {}

void ParticleSystem::Draw() {

	/// 生きているParticleの数を確認
	uint32_t instnanceNum = 0u;
	std::partition(particleArray_.begin(), particleArray_.end(), [&](const std::unique_ptr<Particle>& particle) {
		if(particle->GetIsAlive()) {
			instnanceNum++;
			return true;
		}
		return false;
	});

	if(instnanceNum > 0) {
		sPipeline_->Draw(particleArray_, instnanceNum);
	}
}




/// ===================================================
/// ParticlePipeline 
/// ===================================================

void ParticlePipeline::Initialize(ID3D12GraphicsCommandList* commandList) {

	/// pointer set
	pCommandList_ = commandList;
	assert(pCommandList_);


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
	pipelineState_->AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	/// Constant Buffer
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	///- viewProjection
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 1);	///- transform
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);	///- material
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);	///- directional light

	/// SRV - Texture
	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 0);
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	/// Pipeline Create
	pipelineState_->Initialize();

}



void ParticlePipeline::Draw(const std::vector<std::unique_ptr<class Particle>>& particleArray, uint32_t instanceCount) {

	//pCommandList_->DrawIndexedInstanced(4, instanceCount, 0, 0, 0);
}
