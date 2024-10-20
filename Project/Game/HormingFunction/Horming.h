#pragma once
#include"Math/Vector3.h"
#include"Math/Quaternion.h"

Vec3 QuaternionToEulerAngles(const Quaternion& q);

Vec3 DealtWithZimbalLock(float x, const Quaternion& q);

//Vector3からQuaternion変換
Quaternion ToQuaternion(const Vec3& eulerAngles);

std::pair<float, float> CalculateDistanceAndDirection(const Vec3& targetPos, const Vec3& bossPosition, const float& radius);

// QuaternionのLerp関数
Quaternion Lerp(const Quaternion& start, const Quaternion& end, float t);

// Slerp関数（最短距離で補間）
Quaternion Slerp(const Quaternion& start, Quaternion end, float t);