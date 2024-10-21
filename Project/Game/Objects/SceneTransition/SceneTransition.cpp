#define NOMINMAX
#include "SceneTransition.h"


/// engine
#include "FrameManager/Time.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

/// math
#include "Math/Easing.h"


SceneTransition::SceneTransition(int _transitionOrder) {
	CreateTag(this);
	SetTransitionOrder(_transitionOrder);
}

SceneTransition::~SceneTransition() {}

void SceneTransition::Initialize() {

	drawLayerId = 1; /// ui layer

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("uvChecker.png");

	/// transition orderに合わせた初期化
	if(transitionOrder_ == TRANSTION_FADE_IN) {
		pTransform_->scale = {};
	} else {
		pTransform_->scale = Vec3::kOne * 10.0f;
	}
	
}

void SceneTransition::Update() {

	/// timeの更新
	animationTime_ += Time::DeltaTime();

	if(transitionOrder_ == TRANSTION_FADE_IN) {
		UpdateFadeIn();
	} else {
		UpdateFadeOut();
	}
}

void SceneTransition::SetTransitionOrder(int _transitionOrder) {
	transitionOrder_ = _transitionOrder;
}

bool SceneTransition::GetIsFinished() const {
	return (animationTime_ / maxAnimationTime_) >= 1.0f;
}

void SceneTransition::UpdateFadeIn() {
	float easeT = std::min(animationTime_ / maxAnimationTime_, 1.0f);

	float scaleScaler = std::lerp(
		0.0f, 10.0f,
		Ease::In::Back(easeT)
	);

	pTransform_->scale = Vec3::kOne * scaleScaler;

}


void SceneTransition::UpdateFadeOut() {
	float easeT = std::min(animationTime_ / maxAnimationTime_, 1.0f);

	float scaleScaler = std::lerp(
		10.0f, 0.0f,
		Ease::In::Back(easeT)
	);

	pTransform_->scale = Vec3::kOne * scaleScaler;
}
