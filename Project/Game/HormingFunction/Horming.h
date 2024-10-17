#pragma once
#include"Math/Vector3.h"
#include"Math/Quaternion.h"

Vec3 QuaternionToEulerAngles(const Quaternion& q);

Vec3 DealtWithZimbalLock(float x, const Quaternion& q);

//Vector3からQuaternion変換
Quaternion ToQuaternion(const Vec3& eulerAngles);

std::pair<float, float> CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition, const float& radius);
