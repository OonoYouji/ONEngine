#include "DeadEffect.h"

/// engine
#include <imgui.h>
#include "FrameManager/Time.h"
#include "Input/Input.h"

/// components
#include "ComponentManager/ParticleSystem/ParticleSystem.h"

/// math
#include "Math/Random.h"
#include "Math/Easing.h"


DeadEffect::DeadEffect() {
	CreateTag(this);
}

DeadEffect::~DeadEffect() {}

void DeadEffect::Initialize() {
	
	const size_t kMaxParticleCount= 12;

	particleSyste_ = AddComponent<ParticleSystem>(static_cast<uint32_t>(kMaxParticleCount), "Bom");
	

	particleSyste_->SetEmittedParticleCount(2);
	particleSyste_->SetParticleLifeTime(1.0f);
	particleSyste_->SetParticleRespawnTime(0.3f);
	particleSyste_->SetUseBillboard(false);
	particleSyste_->SetBoxEmitterMinMax({}, {});

	/// particle dataの初期化
	particleDataArray_.resize(kMaxParticleCount);
	for(auto& data : particleDataArray_) {
		data.position = Random::Vec3(-Vec3::kOne * 3.0f, Vec3::kOne * 3.0f);
		data.scaleScaler = 0.0f;
	}

	/// 更新処理の関数をセットする
	particleSyste_->SetPartilceUpdateFunction([&](Particle* particle) {

		Transform* transform = particle->GetTransform();
		ParticleData& data = particleDataArray_[particle->GetID()];
		transform->SetParent(pTransform_);

		data.scaleScaler = (1.0f - particle->GetNormLifeTime()) * 0.5f;
		transform->position = data.position;
		transform->scale = Vec3::kOne * data.scaleScaler;

		if(!particle->GetIsAlive() || particle->GetLifeTime() <= 0.0f) {
			data.position = Random::Vec3(-Vec3::kOne, Vec3::kOne);
			data.scaleScaler = 0.0f;
			transform->scale = Vec3::kOne * data.scaleScaler;
			transform->position = data.position;
		}

	});


	particleSyste_->isActive = false;
}

void DeadEffect::Update() {

	if(isStart_) {

		endTime_ -= Time::DeltaTime();

	}
}

void DeadEffect::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		for(size_t i = 0; i < particleDataArray_.size(); ++i) {
			
			std::string label = std::string("data_") + std::to_string(i);
			if (ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)){
				
				ParticleData& data = particleDataArray_[i];
				ImGui::DragFloat("scale scaler", &data.scaleScaler, 0.1f);
				ImGui::DragFloat3("position", &data.position.x, 0.1f);

				ImGui::TreePop();
			}

			ImGui::Spacing();

		}


		ImGui::TreePop();
	}
}

void DeadEffect::SetIsStart(bool _isStart) {
	isStart_ = _isStart;
	particleSyste_->isActive = isStart_;
}
