#define NOMINMAX
#include "SceneTransition.h"

/// engine
#include "FrameManager/Time.h"

/// component
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"


SceneTransition::SceneTransition(TRANSITION_TYPE _type, float _animationTime, uint32_t _drawLayerId) {
	CreateTag(this);

	transitionType_   = _type;
	maxAnimationTime_ = _animationTime;
	drawLayerId       = _drawLayerId;
}

SceneTransition::~SceneTransition() {}

void SceneTransition::Initialize() {
	
	spriteRenderer_ = AddComponent<SpriteRenderer>();
	spriteRenderer_->SetTexture("white2x2");

	color_ = Vec4(0, 0, 0, 1);
	spriteRenderer_->SetColor(color_);

}

void SceneTransition::Update() {

	currentAnimationTime_ += Time::DeltaTime();

	/// transition typeごとに更新処理を分ける
	switch(transitionType_) {
	case TRANSITION_TYPE_IN:
		TransitionIn();
		break;
	case TRANSITION_TYPE_OUT:
		TransitionOut();
		break;
	}

	spriteRenderer_->SetColor(color_);

}

void SceneTransition::TransitionIn() {

	/// 線形補完用 T
	float lerpT = std::min(currentAnimationTime_ / maxAnimationTime_, 1.0f);
	color_.w = std::lerp(0.0f, 1.0f, lerpT);

	if(lerpT == 1.0f) {
		isEnd_ = true;
	}
}

void SceneTransition::TransitionOut() {
	/// 線形補完用 T
	float lerpT = std::min(currentAnimationTime_ / maxAnimationTime_, 1.0f);
	color_.w = std::lerp(1.0f, 0.0f, lerpT);

	if(lerpT == 1.0f) {
		isEnd_ = true;
	}

}


