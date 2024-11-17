#include "MovingLight.h"

// std
#include <numbers>

/// engine
#include "FrameManager/Time.h"
#include "Math/Random.h"

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

MovingLight::MovingLight(const Vec3& wPosition) : BaseBackgroundObject(wPosition) {
	CreateTag(this);
}

MovingLight::~MovingLight() {}

void MovingLight::Initialize() {
	animationRenderer_ = AddComponent<AnimationRenderer>("MovingLight");

	Vec3 rotate = 2.0f * std::numbers::pi_v<float> * Vec3::kOne;
	pTransform_->rotate = Random::Vec3(-rotate, rotate);
}

void MovingLight::Update() {

	animationTime_ += Time::DeltaTime();
	if(animationRenderer_->GetDuration("MovingLight") <= animationTime_) {
		Destory();
	}

}

