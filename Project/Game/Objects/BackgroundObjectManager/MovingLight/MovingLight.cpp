#include "MovingLight.h"

/// component
#include "ComponentManager/AnimationRenderer/AnimationRenderer.h"

MovingLight::MovingLight(const Vec3& wPosition) : BaseBackgroundObject(wPosition) {
	CreateTag(this);
}

MovingLight::~MovingLight() {}

void MovingLight::Initialize() {
	animationRenderer_ = AddComponent<AnimationRenderer>("MovingLight");

}

void MovingLight::Update() {

}

