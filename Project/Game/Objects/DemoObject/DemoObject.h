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
	
	Vec3  axis_;
	float angle_;
	Mat4  matRotata_;

};
