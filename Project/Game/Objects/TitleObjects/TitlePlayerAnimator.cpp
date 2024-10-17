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
	meshRenderer->SetModel("axis");

	EarthRenderer* earthRenderer = AddComponent<EarthRenderer>();
	earthRenderer->SetRadius(1.5f);
	earthRenderer->SetColor({0,0,0, 0.75f});

	//ParticleSystem* particleSystem = AddComponent<ParticleSystem>();

	windArray_.resize(10);
	for(auto& wind : windArray_) {
		wind = new Wind;
		wind->Initialize();
		wind->SetParent(pTransform_);
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


	/// パーティクルの挙動

	/*particleSystem->SetPartilceUpdateFunction([&](Particle* particle) {

	});*/


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
