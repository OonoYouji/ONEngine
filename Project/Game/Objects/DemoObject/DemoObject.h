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

	void OnCollisionEnter(BaseGameObject* _other) override;

private:


};


std::pair<Vec3, Vec3> ComputeCollisionVelocities(
	float _mass1, const Vec3& _velocity1, 
	float _mass2, const Vec3& _velocity2, 
	float _coefficietRestituion, const Vec3& _normal
);