#include "TitlePlayerAnimator.h"

/// std
#include <numbers>

/// engine
#include "imgui.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/ParticleSystem/ParticleSystem.h"
#include "CustomComponents/EarthRenderer/EarthRenderer.h"

/// objects
#include "Objects/Tornado/Tornado.h"

/// math
#include "Math/Random.h"
#include "Math/Easing.h"



void TitlePlayerAnimator::Initialize() {

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("player");

	if(useShadow_) {
		EarthRenderer* earthRenderer = AddComponent<EarthRenderer>();
		earthRenderer->SetRadius(1.5f);
		earthRenderer->SetColor({0,0,0, 0.75f});
	}

	particleSystem_ = AddComponent<ParticleSystem>(64u, "rubble");
	particleDataArray_.resize(64u);

	windArray_.resize(10);
	for(auto& wind : windArray_) {
		wind = new Wind;
		wind->Initialize();
		wind->SetParent(pTransform_);
		wind->SetScale(Vec3::kOne * 0.75f);
	}

	windAnimationDataArray_.resize(10);
	for(auto& data : windAnimationDataArray_) {
		data.speed = Random::Float(1.0f, 4.0f) * 4.0f;
	}


	/// transform setting...
	basePosition_         = { 0.0f, 10.8f, 0.0f };
	pTransform_->position = basePosition_;
	pTransform_->scale = Vec3::kOne * 0.5f;

	/// animation setting
	animationTime_      = 0.0f;
	animationSpeed_     = 4.0f;
	animationAmplitude_ = 0.4f;



	/// ---------------------------------------------------
	/// 飛散物 
	/// ---------------------------------------------------

	/// パーティクルデータの初期化
	for(auto& data : particleDataArray_) {
		data.maxPosY = Random::Float(1.0f, 10.0f);
		data.radius  = Random::Float(1.0f, 2.0f);
		data.speed   = Random::Float(5.0f, 10.0f);
		data.time    = Random::Float(0.0f, 1.0f);
		data.rotate  = Random::Vec3(-Vec3::kOne, Vec3::kOne);
		data.scale = Random::Vec3(Vec3::kOne * 0.1f, Vec3::kOne * 0.5f);
	}

	/// パーティクルの挙動
	particleSystem_->SetParticleLifeTime(3.0f);
	particleSystem_->SetEmittedParticleCount(2);
	particleSystem_->SetParticleRespawnTime(0.3f);
	particleSystem_->SetUseBillboard(false);
	useRotate_ = true;
	
	particleSystem_->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();
		ParticleData& data = particleDataArray_[particle->GetID()];

		data.time += Time::DeltaTime();
		transform->rotate = data.rotate;
		transform->scale = data.scale;

		transform->position = {
			std::cos(-data.time * data.speed) * data.radius,
			(1.0f - particle->GetNormLifeTime()) * data.maxPosY,
			std::sin(-data.time * data.speed) * data.radius
		};

		if(useRotate_) {
			transform->position = Mat4::Transform(transform->position, matRotate_);
		}

		transform->position += GetPosition();
	});


	/// ---------------------------------------------------
	/// 風のエフェクト 
	/// ---------------------------------------------------

	const uint32_t kWindSize = 6u;

	windDataArray_.resize(kWindSize);
	for(size_t i = 0; i < kWindSize; ++i) {
		WindData& wind = windDataArray_[i];
		wind.time   = Random::Float(1.0f, 3.0f);
		wind.speed  = Random::Float(5.0f, 7.5f) * 1.5f;
		wind.radius = Random::Float(1.0f, 3.0f);
		wind.height = Random::Float(1.0f, 5.0f);
	}

	ParticleSystem* windParticle = AddComponent<ParticleSystem>(kWindSize, "wind");
	windParticle->SetParticleLifeTime(1.0f);
	windParticle->SetParticleRespawnTime(0.2f);
	windParticle->SetEmittedParticleCount(1);
	windParticle->SetUseBillboard(false);

	windParticle->SetPartilceUpdateFunction([&](Particle* particle) {
		Transform* transform = particle->GetTransform();
		WindData& wind = windDataArray_[particle->GetID()];

		wind.time += Time::DeltaTime();

		if(useRotate_) {

			transform->position = {
				std::cos(-wind.time * wind.speed) * wind.height * 0.25f,
				std::sin(-wind.time * wind.speed) * wind.height * 0.25f,
				-wind.height - 1.0f
			};

			transform->rotate.x = std::numbers::pi_v<float> * 0.5f;
			transform->rotate.z = std::atan2(-transform->position.y, -transform->position.x);

			transform->scale = Vec3::kOne * wind.height * 0.25f;
			transform->scale.y = 2.0f;
		} else {

			transform->position = {
				std::cos(-wind.time * wind.speed),
				wind.height - 1.0f,
				std::sin(-wind.time * wind.speed)
			};

			transform->rotate.y = std::atan2(transform->position.x, transform->position.z);
			transform->rotate.y += std::numbers::pi_v<float> * 0.5f;

			transform->scale = Vec3::kOne * wind.height * 0.25f;
			transform->scale.y = 2.0f;
		}

		transform->position += basePosition_;

		/// reset ...
		if(particle->GetNormLifeTime() <= 0.0f) {
			wind.height = Random::Float(1.0f, 5.0f);
		}
	});

}

void TitlePlayerAnimator::Update() {

	/// 周りの風を回転させる
	for(size_t i = 0; i < 10; ++i) {
		WindAnimationData& data = windAnimationDataArray_[i];
		Wind* wind = windArray_[i];

		data.time += Time::DeltaTime();
		wind->SetRotateY(data.time * data.speed);
	}

	
	/// thisが回転する挙動
	if(isSpinUpdate_) {
		if(isSpin_) {
			spinTime_ += Time::DeltaTime();
			pTransform_->rotate.y = (std::numbers::pi_v<float> * 2.0f) * 
				Ease::InOut::Elastic(spinTime_ / 2.0f);
	
			if(spinTime_ >= 2.0f) {
				isSpin_ = false;
				spinTime_ = 0.0f;
			}
		} else {
			if(animationTime_ > 1.0f && static_cast<int>(animationTime_) % 4 == 0) {
				isSpin_ = true;
			}
		}
	}

	/// 上下に動く挙動
	animationTime_ += Time::DeltaTime();
	Vec3 offset = {
		0.0f,
		std::sin(animationSpeed_ * animationTime_) * 0.5f + 0.5f,
		0.0f
	};

	offset = Mat4::Transform(offset, Mat4::MakeRotate(pTransform_->rotate));
	offset *= animationAmplitude_;

	pTransform_->position = basePosition_ + offset;
	matRotate_ = Mat4::MakeRotate(pTransform_->rotate);
}

void TitlePlayerAnimator::Debug() {
	if(ImGui::TreeNodeEx("debug", ImGuiTreeNodeFlags_DefaultOpen)) {

		if(ImGui::TreeNodeEx("animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		
			/// animation
			ImGui::DragFloat("time",      &animationTime_,      0.01f);
			ImGui::DragFloat("speed",     &animationSpeed_,     0.01f);
			ImGui::DragFloat("amplitude", &animationAmplitude_, 0.01f);

			ImGui::Spacing();
		
			/// position 
			ImGui::DragFloat3("base position",   &basePosition_.x,   0.01f);

			ImGui::Spacing();

			/// save
			if(ImGui::Button("var save")) {
				/// variable json output...
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void TitlePlayerAnimator::SetBasePosition(const Vec3& _basePosition) {
	basePosition_ = _basePosition;
}

void TitlePlayerAnimator::SetIsSpinUpdate(bool isSpinUpdate) {
	isSpinUpdate_ = isSpinUpdate;
}

void TitlePlayerAnimator::SetParticleUseRotate(bool _useRotate) {
	useRotate_ = _useRotate;
}

void TitlePlayerAnimator::SetUseShadow(bool _useShadow) {
	useShadow_ = _useShadow;
}
