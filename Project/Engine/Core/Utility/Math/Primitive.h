#pragma once

/// std
#include <array>

/// engine
#include "Vector3.h"

/// @brief 球
namespace ONEngine {

struct Sphere {
	Vector3 center;
	float radius;
};

/// @brief 箱
struct Cube {
	Vector3 center;
	Vector3 size;
};

/// @brief 線分
struct Line {
	Vector3 start;
	Vector3 end;
};

/// @brief レイ(光)
struct Ray {
	Vector3 origin;
	Vector3 direction;
};

/// @brief 平面
struct Plane {
	/// @brief 面の法線
	Vector3 normal;
	/// @brief 面から原点までの距離
	float d;
};

/// @brief 視錐台
struct Frustum {
	static constexpr size_t kPlaneCount = 6;
	std::array<Plane, kPlaneCount> planes;
};


} /// ONEngine
