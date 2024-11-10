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

	void OnCollisionStay(BaseGameObject* const _collision) override;
	void OnCollisionEnter(BaseGameObject* const _collision) override;
	void OnCollisionExit(BaseGameObject* const _collision) override;

private:
	class MeshRenderer* meshRenderer_ = nullptr;
	class NumberRenderer* numberRenderer_ = nullptr;
	std::array<Vec3, 2> positionArray_;


	uint32_t score_ = 0u;;
};
