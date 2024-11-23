#pragma once
#include "IEnemyBullet.h"

class LongRangeBullet :
	public IEnemyBullet{
public:
	LongRangeBullet();
	~LongRangeBullet(){}

	/// ===================================================
	/// public : methods
	/// ===================================================

	void Initialize() override;
	void Update()     override;
private:
	// XZ 方面の velocity
	Vector2 velocityXZ_;
public:
	void SetVelocityXZ(const Vector2& velocity){ velocityXZ_ = velocity; }
};