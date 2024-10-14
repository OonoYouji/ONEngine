#include "TitlePlayerAnimator.h"

/// engine
#include "imgui.h"
#include "FrameManager/Time.h"

/// components
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/Tornado/Tornado.h"


void TitlePlayerAnimator::Initialize() {

	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetModel("axis");

	for(size_t i = 0; i < 3; ++i) {
		Ring* ring = new Ring;
		ring->Initialize();
		ring->SetParent(pTransform_);
	}

	/// transform setting...
	basePosition_         = { 0.0f, 11.4f, 0.0f };
	pTransform_->position = basePosition_;


	/// animation setting
	animationTime_      = 0.0f;
	animationSpeed_     = 4.0f;
	animationAmplitude_ = 0.4f;

}

void TitlePlayerAnimator::Update() {

	animationTime_ += Time::DeltaTime();

	/// sin, cosの計算結果を0~1に修正する
	Vec3 offset = {
		0.0f,
		std::sin(animationSpeed_ * animationTime_) * 0.5f + 0.5f,
		0.0f
	};

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
