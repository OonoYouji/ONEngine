#define NOMINMAX
#include "ParticleSystem.h"

/// std
#include <cassert>
#include <algorithm>
#include <numbers>

/// engine
#include "Core/ONEngine.h"
#include "ImGuiManager/ImGuiManager.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"

#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"
#include "GraphicManager/ModelManager/ModelManager.h"
#include "GraphicManager/Light/DirectionalLight.h"
#include "FrameManager/Time.h"
#include "GraphicManager/Light/LightGroup.h"
#include "Objects/Camera/Manager/CameraManager.h"
#include "SceneManager/SceneManager.h"

/// math
#include "Math/Random.h"


/// static object intialize
std::unique_ptr<ParticlePipeline> ParticleSystem::sPipeline_ = nullptr;




/// ===================================================
/// ParticleSystem 
/// ===================================================

ParticleSystem::ParticleSystem(uint32_t maxParticleNum, const std::string& filePath) : kMaxParticleNum_(maxParticleNum) {
	pModel_ = ModelManager::Load(filePath);
}

ParticleSystem::~ParticleSystem() {
	transforms_.reset();
	materials_.reset();
}


void ParticleSystem::SInitialize(ID3D12GraphicsCommandList* pCommandList_) {
	if(!sPipeline_) {
		sPipeline_.reset(new ParticlePipeline);
		sPipeline_->Initialize(pCommandList_);
	}
}

void ParticleSystem::SFinalize() {
	sPipeline_.reset();
}

void ParticleSystem::SetLightGroup(LightGroup* _lightGroup) {
	sPipeline_->SetLightGroup(_lightGroup);
}

void ParticleSystem::PreDraw() {
	sPipeline_->PreDraw();
}

void ParticleSystem::PostDraw() {
	sPipeline_->PostDraw();
}


void ParticleSystem::Initialize() {

	transforms_.reset(new DxStructuredBuffer<Mat4>());
	transforms_->Create(kMaxParticleNum_);

	materials_.reset(new DxStructuredBuffer<Material::MaterialData>());
	materials_->Create(kMaxParticleNum_);
	for(size_t i = 0; i < kMaxParticleNum_; ++i) {
		materials_->SetMappedData(i, *pModel_->GetMaterials().front().GetMaterialData());

	}

	/// particle system setting
	particleArray_.reserve(kMaxParticleNum_);

	/// particle setting
	//SetParticleRespawnTime(0.5f);
	//SetEmittedParticleCount(1u);
	SetParticleLifeTime(5.0f);
	SetUseBillboard(true);

	/// billboard setting
	matBackToFront_ = Mat4::MakeRotateY(std::numbers::pi_v<float>);

	/// particle update function
	particleUpdateFunc_ = [&](Particle* particle) {
		Transform* tf = particle->GetTransform();

		Vec3 randomDir = Random::Vec3(-Vec3::kOne, Vec3::kOne).Normalize();
		tf->position += randomDir * Time::DeltaTime();
	};


	/// emitter
	emitter_.reset(new ParticleEmitter);
	emitter_->Initialize(&particleArray_, this);

}



void ParticleSystem::Update() {

	/// ビルボード用行列の作成
	if(useBillboard_) {
		auto mainCamera = CameraManager::GetInstance()->GetCamera("MainCamera");
		matBillboard_   = matBackToFront_ * mainCamera->GetMatTransform();
		matBillboard_.m[3][0] = 0.0f;
		matBillboard_.m[3][1] = 0.0f;
		matBillboard_.m[3][2] = 0.0f;
	}

	for(size_t i = 0; i < particleArray_.size(); ++i) {
		Particle* particle = particleArray_[i].get();
		Transform* transform = particle->GetTransform();

		if(!particle->isAlive_) {
			continue;
		}

		/// filedに当たっていれば処理を行う
		for(auto itr = pFieldArray_.begin(); itr != pFieldArray_.end(); ++itr) {
			ParticleField* field = (*itr);

			/// min以下にある場合はcontinue
			if(field->GetMin().x > transform->position.x
				|| field->GetMin().y > transform->position.y
				|| field->GetMin().z > transform->position.z) {
				continue;
			}

			/// max以上にある場合はcontinue
			if(transform->position.x > field->GetMax().x
			   || transform->position.y > field->GetMax().y
			   || transform->position.z > field->GetMax().z) {
				continue;
			}

			field->Update(particle);
			
		}


		particle->LifeTimeUpdate();
		particle->id_ = static_cast<uint32_t>(i);
		particleUpdateFunc_(particle);

		if(useBillboard_) {

			Mat4 matScale = Mat4::MakeScale(particle->transform_.scale);
			Mat4 matTranslate = Mat4::MakeTranslate(particle->transform_.position);

			particle->transform_.matTransform = matScale * matBillboard_ * matTranslate;

			Transform* parent = particle->transform_.GetParent();
			if(parent) {
				particle->transform_.matTransform *= parent->matTransform;
			}

		} else {
			particle->transform_.Update();
		}
	}

	emitter_->Update();
}

void ParticleSystem::Draw() {
	sPipeline_->AddActiveParticleSystem(this);
}

void ParticleSystem::Debug() {
	if(ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::DragFloat("particle life time",            &particleLifeTime_, 0.05f, 0.0f, 60.0f);
		ImGui::Checkbox("use billboard", &useBillboard_);

		if(ImGui::Button("burst particle")) {
			SetBurst(true, 1.0f, 0.1f);
		}

		ImGui::Spacing();

		emitter_->Debug();

		ImGui::TreePop();
	}
}


void ParticleSystem::DrawCall() {
	emitter_->currentParticleCount_ = 0u;
	std::vector<Mat4> matTransformArray;

	/// 生きているParticleの数を確認
	std::partition(particleArray_.begin(), particleArray_.end(), [&](const std::unique_ptr<Particle>& particle) {
		if(particle->GetIsAlive()) {
			emitter_->currentParticleCount_++;
			matTransformArray.push_back(particle->GetTransform()->matTransform);
			return true;
		}
		return false;
	});

	for(size_t i = 0; i < particleArray_.size(); ++i) {
		materials_->SetMappedData(i, particleArray_[i]->GetMaterial());
	}


	/// 描画処理に移行する
	if(emitter_->currentParticleCount_ > 0) {
		for(size_t i = 0; i < matTransformArray.size(); ++i) {
			transforms_->SetMappedData(i, matTransformArray[i]);
		}

		sPipeline_->Draw(transforms_.get(), materials_.get(), pModel_, emitter_->currentParticleCount_);
	}
}

void ParticleSystem::SetParticleRespawnTime(float _particleRespawnTime) {
	emitter_->rateOverTime_ = _particleRespawnTime;
}

void ParticleSystem::SetEmittedParticleCount(uint32_t _emittedParticleCount) {
	emitter_->emissionCount_ = _emittedParticleCount;
}

void ParticleSystem::SetParticleLifeTime(float _particleLifeTime) {
	particleLifeTime_ = _particleLifeTime;
}

void ParticleSystem::SetUseBillboard(bool _useBillboard) {
	useBillboard_ = _useBillboard;
}


void ParticleSystem::SetPartilceUpdateFunction(const std::function<void(Particle*)>& _function) {
	particleUpdateFunc_ = _function;
}

void ParticleSystem::SetParticleEmitterFlags(int particleEmitterFlags) {
	emitter_->SetParticleEmitterFlags(particleEmitterFlags);
}

void ParticleSystem::SetBurst(bool _isBurst, float _burstTime, float _rateOverTime) {
	emitter_->SetBurst(_isBurst, _burstTime, _rateOverTime);
}

void ParticleSystem::SetBoxEmitterMinMax(const Vec3& _min, const Vec3& _max) {
	emitter_->min_ = _min;
	emitter_->max_ = _max;
}

void ParticleSystem::SetFieldArray(const std::list<ParticleField*>& _filedArray) {
	pFieldArray_ = _filedArray;
}

void ParticleSystem::AddField(ParticleField* _field) {
	pFieldArray_.push_back(_field);
}






/// ===================================================
/// ParticlePipeline 
/// ===================================================

void ParticlePipeline::Initialize(ID3D12GraphicsCommandList* commandList_) {

	/// pointer set
	pCommandList_ = commandList_;
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
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);	/// viewProjection
	pipelineState_->AddCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);	/// camera 

	/// descriptor
	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV - Transform
	pipelineState_->AddDescriptorRange(0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV - Texture
	pipelineState_->AddDescriptorRange(1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV - Directional Light
	pipelineState_->AddDescriptorRange(2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV - Point Light
	pipelineState_->AddDescriptorRange(3, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); /// SRV - Material

	/// SRV - Transform
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_VERTEX, 0);

	/// SRV - Texture
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 1);
	pipelineState_->AddStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	/// SRV - Light
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 2);
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 3);
	
	/// SRV - Material
	pipelineState_->AddDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL, 4);



	/// Pipeline Create
	pipelineState_->Initialize();

}


void ParticlePipeline::Update() {}



void ParticlePipeline::Draw(
	DxStructuredBuffer<Mat4>* _transformBuffer, 
	DxStructuredBuffer<Material::MaterialData>* _materialBuffer, 
	Model* useModel, uint32_t instanceCount) {

	/// other pointer get
	BaseCamera* pCamera = CameraManager::GetInstance()->GetMainCamera();


	/// default setting
	pipelineState_->SetPipelineState();

	/// command setting
	pCommandList_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList_->SetGraphicsRootConstantBufferView(0, pCamera->GetViewBuffer()->GetGPUVirtualAddress());
	pCommandList_->SetGraphicsRootConstantBufferView(1, pCamera->GetPositionBuffer()->GetGPUVirtualAddress());

	pLightGroup_->BindDirectionalLightBufferForCommandList(4, pCommandList_);
	pLightGroup_->BindPointLightBufferForCommandList(5, pCommandList_);

	_transformBuffer->BindToCommandList(2, pCommandList_); /// bind transform
	_materialBuffer->BindToCommandList(6, pCommandList_);  /// bind material

	for(auto& material : useModel->GetMaterials()) {
		//material.BindMaterial(pCommandList_, 1);
		material.BindTexture(pCommandList_, 3);
	}


	/// 描画コールの呼び出し
	for(auto& mesh : useModel->GetMeshes()) {

		mesh.Draw(pCommandList_, false);

		UINT indexCountPerInstance = static_cast<UINT>(mesh.GetIndices().size());
		pCommandList_->DrawIndexedInstanced(indexCountPerInstance, instanceCount, 0, 0, 0);
	}
}

void ParticlePipeline::SetLightGroup(LightGroup* _lightGroup) {
	pLightGroup_ = _lightGroup;
}

void ParticlePipeline::PreDraw() {
	activeParticleSystem_.clear();
}

void ParticlePipeline::PostDraw() {

	for (auto& renderer : activeParticleSystem_) {
		renderer->DrawCall();
	}

}





/// ===================================================
/// Particle
/// ===================================================

void Particle::Initialize() {
	//transform_.Initialize();
}


void Particle::LifeTimeUpdate() {
	lifeTime_ -= Time::DeltaTime();
	if(lifeTime_ < 0.0f) {
		isAlive_ = false;
	}
}

