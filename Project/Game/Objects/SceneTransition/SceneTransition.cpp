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

	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("white2x2.png");

	pTransform_->scale = { 8.6f, 7.7f, 1.0f };

	/// transition orderに合わせた初期化
	if(transitionOrder_ == TRANSTION_FADE_IN) {
		color_ = { 0, 0, 0, 0 };
	} else {
		color_ = { 0, 0, 0, 1 };
	}
	
	spriteRenderer_->SetColor(color_);
}

void SceneTransition::Update() {

	if(!isStarted_) {
		return;
	}

	/// timeの更新
	animationTime_ += Time::DeltaTime();

	if(transitionOrder_ == TRANSTION_FADE_IN) {
		UpdateFadeIn();
	} else {
		UpdateFadeOut();
	}

	spriteRenderer_->SetColor(color_);

}

void SceneTransition::SetTransitionOrder(int _transitionOrder) {
	transitionOrder_ = _transitionOrder;
}

bool SceneTransition::GetIsFinished() const {
	return (animationTime_ / maxAnimationTime_) >= 1.0f;
}

void SceneTransition::SetIsStarted(bool _isStart) {
	isStarted_ = _isStart;
}

void SceneTransition::UpdateFadeIn() {
	float easeT = std::min(animationTime_ / maxAnimationTime_, 1.0f);

	float alpha = std::lerp(
		0.0f, 1.0f,
		(easeT)
	);

	color_.w = alpha;

}


void SceneTransition::UpdateFadeOut() {
	float easeT = std::min(animationTime_ / maxAnimationTime_, 1.0f);

	float scaleScaler = std::lerp(
		10.0f, 0.0f,
		Ease::In::Back(easeT)
	);

	pTransform_->scale = Vec3::kOne * scaleScaler;
}
