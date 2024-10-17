#include "TitlePlayerAnimator.h"

/// std
#include <numbers>

/// engine
#include "imgui.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Tornado/Tornado.h"

/// math
#include "Math/Random.h"



void TitlePlayerAnimator::Initialize() {

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("axis");


	windArray_.resize(10);
	for(auto& wind : windArray_) {
		wind = new Wind;
		wind->Initialize();
		wind->SetParent(pTransform_);
	}

	windAnimationDataArray.resize(10);
	for(auto& data : windAnimationDataArray) {
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

}

void TitlePlayerAnimator::Update() {

	for(size_t i = 0; i < 10; ++i) {
		WindAnimationData& data = windAnimationDataArray[i];
		Wind* wind = windArray_[i];

		data.time += Time::DeltaTime();
		wind->SetRotateY(data.time * data.speed);
	}

	
	if(isSpin_) {
		spinTime_ += Time::DeltaTime();
		pTransform_->rotate.y += (std::numbers::pi_v<float> * 2.0f) * (Time::DeltaTime());

		if(spinTime_ >= 0.5f) {
			isSpin_ = false;
			spinTime_ = 0.0f;
		}
	} else {
		if(animationTime_ > 1.0f && static_cast<int>(animationTime_) % 10 == 0) {
			isSpin_ = true;
		}
	}


	animationTime_ += Time::DeltaTime();

	/// sin, cosの計算結果を0~1に修正する
	Vec3 offset = {
		0.0f,
		std::sin(animationSpeed_ * animationTime_) * 0.5f + 0.5f,
		0.0f
	};

	offset = Mat4::Transform(offset, Mat4::MakeRotate(pTransform_->rotate));
	offset *= animationAmplitude_;

	/// 
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
