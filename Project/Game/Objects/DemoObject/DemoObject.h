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
	int id_;
	float speed_;
	bool isAlive_;
	Vec2 size_;
	Vec3 position_;
	Vec4 color_;
};


class DemoObject2 final : public BaseGameObject {
public:

	DemoObject2() { CreateTag(this); }
	~DemoObject2() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:
	int id_;
	float speed_;
	bool isAlive_;
	Vec2 size_;
	Vec3 position_;
	Vec4 color_;
};