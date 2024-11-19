#pragma once

#include <vector>
#include <memory>

/// base class
#include "GameObjectManager/GameObjectManager.h"

#include "ComponentManager/AnimationRenderer/Skeleton.h"

class DemoObject final : public BaseGameObject {
public:

	DemoObject() { CreateTag(this); }
	~DemoObject() {}
	
	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:
	
	class AnimationRenderer* animationRenderer_ = nullptr;
	class CapsuleCollider* capsuleCollider_ = nullptr;
	std::array<Vec3, 2> positionArray_;

	std::string name_;
};
