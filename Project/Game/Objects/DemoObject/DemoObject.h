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

	void OnCollisionEnter(BaseGameObject* const _collision) override;
	void OnCollisionStay(BaseGameObject* const _collision) override;
	void OnCollisionExit(BaseGameObject* const _collision) override;

private:
	
	class AnimationRenderer* animationRenderer_ = nullptr;

	std::string name_;

	bool isCollisionEnter_= false;
	bool isCollisionStay_= false;
	bool isCollisionExit_= false;
};
