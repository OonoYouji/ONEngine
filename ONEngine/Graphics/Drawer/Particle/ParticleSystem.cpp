#include "ParticleSystem.h"

#include <cassert>
#include <algorithm>

#include <CameraManager.h>
#include <SceneManager.h>
#include <ModelManager.h>

#include <Light/DirectionalLight.h>


std::unique_ptr<ParticlePipeline> ParticleSystem::sPipeline_ = nullptr;




/// ===================================================
/// ParticleSystem 
/// ===================================================

ParticleSystem::ParticleSystem(uint32_t maxParticleNum, const std::string& filePath) : kMaxParticleNum_(maxParticleNum) {
	pModel_ = ModelManager::Load(filePath);
}


void ParticleSystem::SInitialize(ID3D12GraphicsCommandList* pCommandList_, ONE::DxDescriptor* dxDescriptor) {
	if(!sPipeline_) {
		sPipeline_.reset(new ParticlePipeline);
		sPipeline_->Initialize(pCommandList_, dxDescriptor);
	}
}

void ParticleSystem::SFinalize() {
	sPipeline_.reset();
}


void ParticleSystem::Initialize() {
	particleArray_.reserve(kMaxParticleNum_);
	for(uint32_t i = 0u; i < 2; ++i) {
		particleArray_.push_back(std::make_unique<Particle>());
		particleArray_.back()->Initialize();
	}

}



void ParticleSystem::Update() {
	for(auto& particle : particleArray_) {
		particle->Update();
	}

}

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
		sPipeline_->Draw(particleArray_, pModel_, instnanceNum);
	}
}




/// ===================================================
/// ParticlePipeline 
/// ===================================================

void ParticlePipeline::Initialize(ID3D12GraphicsCommandList* commandList_, ONE::DxDescriptor* dxDescriptor) {

	/// pointer set
	pCommandList_ = commandList_;
	assert(pCommandList_);

	pDxDescriptor_ = dxDescriptor;
	assert(pDxDescriptor_);


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



void ParticlePipeline::Draw(const std::vector<std::unique_ptr<class Particle>>& particleArray, Model* useModel, uint32_t instanceCount) {

	/// other pointer get
	ID3D12Resource*   viewBuffer = CameraManager::GetInstance()->GetMainCamera()->GetViewBuffer();
	DirectionalLight* pLight     = SceneManager::GetInstance()->GetDirectionalLight();

	/// default setting
	pDxDescriptor_->SetSRVHeap(pCommandList_);
	pipelineState_->SetPipelineState();

	/// command setting
	pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList_->SetGraphicsRootConstantBufferView(0, viewBuffer->GetGPUVirtualAddress());
	pLight->BindToCommandList(3, pCommandList_);


	for(auto& particle : particleArray) {
		particle->transform_.BindTransform(pCommandList_, 1);
	}

	for(auto& material : useModel->GetMaterials()) {
		material.BindMaterial(pCommandList_, 2);
		material.BindTexture(pCommandList_, 4);
	}

	/// 描画コールの呼び出し
	for(auto& mesh : useModel->GetMeshes()) {

		mesh.Draw(pCommandList_, false);

		UINT indexCountPerInstance = static_cast<UINT>(mesh.GetIndices().size());
		pCommandList_->DrawIndexedInstanced(indexCountPerInstance, 1, 0, 0, 0);
	}
}




/// ===================================================
/// Particle
/// ===================================================

void Particle::Initialize() {
	transform_.Initialize();
}


void Particle::Update() {

	transform_.position.z += 0.02f;

	transform_.Update();
}

