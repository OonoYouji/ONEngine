#define NOMINMAX
#include "DefeatStamp.h"

/// engine 
#include "FrameManager/Time.h"
#include <imgui/imgui.h>

/// components
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// math
#include "Math/Easing.h"


DefeatStamp::DefeatStamp() {
	CreateTag(this);
}

DefeatStamp::~DefeatStamp() {}

void DefeatStamp::Initialize() {
	drawLayerId = 1;

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("resultSubjugation.png");

	/// transform
	pTransform_->position = { 1.0f, 0.5f, 0.0f };
	pTransform_->scale = { 4.0f, 2.3f, 1.0f };

	
	stamp_ = new Stamp();
	stamp_->Initialize();
	stamp_->drawLayerId = 1;
}

void DefeatStamp::Update() {

}


/// ===================================================
/// ここからスタンプ用のクラス
/// ===================================================

Stamp::Stamp() {
	CreateTag(this);
}

Stamp::~Stamp() {}

void Stamp::Initialize() {
	drawLayerId = 1;

	spriteRenderer_ = AddComponent<SpriteRenderer>();


	/*##########################################################
		TODO : COMMENT
		ゲームクリア、ゲームオーバーで初期化する値を変える
	##########################################################*/

	spriteRenderer_->SetTexture("resultSubjugationClear.png");
	
	
	/// transform
	pTransform_->position = { 1.5f, 0.7f, 0.0f};
	pTransform_->rotate.z = -0.45f;


}

void Stamp::Update() {

	waitTime_ -= Time::DeltaTime();
	if(waitTime_ > 0.0f) {
		return;
	}

	animationTime_ += Time::DeltaTime();
	float lerpT = std::min(animationTime_ / maxAnimationTime_, 1.0f);

	pTransform_->position = Vec3::Lerp(
		{}, { 1.5f, 0.7f, 0.0f },
		Ease::In::Back(lerpT)
	);

	pTransform_->scale = Vec3::Lerp(
		Vec3::kOne * 4.0f, Vec3::kOne,
		Ease::In::Back(lerpT)
	);
}

void Stamp::Debug() {
	if(ImGui::TreeNodeEx("label", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::SeparatorText("animation");
		ImGui::DragFloat("wait time", &waitTime_, 0.05f);
		ImGui::DragFloat("time", &animationTime_, 0.05f);
		ImGui::DragFloat("max time", &maxAnimationTime_, 0.05f);


		if(ImGui::Button("reset")) {
			waitTime_ = 0.5f;
			animationTime_ = 0.0f;
			maxAnimationTime_ = 1.0f;
		}

		ImGui::TreePop();
	}
}
