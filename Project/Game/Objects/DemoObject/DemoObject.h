#pragma once

#include <vector>
#include <memory>

/// base class
#include "GameObjectManager/GameObjectManager.h"

#include "ComponentManager/AnimationRenderer/Skeleton.h"

class DemoObject : public BaseGameObject {
public:

	DemoObject() { CreateTag(this); }
	~DemoObject() {}
	
	void Initialize() override;
	void Update()     override;
	void Debug()      override;

protected:

	class MeshRenderer* renderer_ = nullptr;

	float mass_;
	Vec3 velocity_;


public:

	float GetMass() const { return mass_; }

	const Vec3& GetVelocity() const { return velocity_; }
	void SetVelocity(const Vec3& _velocity) { velocity_ = _velocity; }

};


std::pair<Vec3, Vec3> ComputeCollisionVelocities(
	float _mass1, const Vec3& _velocity1, 
	float _mass2, const Vec3& _velocity2, 
	float _coefficietRestituion, const Vec3& _normal
);



/// @brief 赤い球のクラス
class RedBall final : public DemoObject {
public:
	RedBall() { CreateTag(this); }
	~RedBall() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void OnCollisionEnter(BaseGameObject* _other) override;

};


/// @brief 白い球のクラス
class WhiteBall final : public DemoObject {
public:
	WhiteBall() { CreateTag(this); }
	~WhiteBall() {}

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void OnCollisionEnter(BaseGameObject* _other) override;

};