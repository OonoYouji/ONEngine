#pragma once 
#include<list>

#include"Building.h"
#include"Player/Player.h"
#include <Component/Transform/Transform.h>

class BuildingManager {
private:

	//パーティクルリスト
	std::list<std::unique_ptr<BaseBuilding>> buildings_;
	Vec3 spownPos_;
	Vec3 basePos_;

public:
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);
	Quaternion LockAt(const Vec3& position, const Vec3& target, const Vec3& up) const;

	void SpownBuilding(float theta, float phi);
	void 	Init();
	void 	Update(Player*player);
	void 	Draw();
};
