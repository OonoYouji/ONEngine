#pragma once

/// engine
#include "Vector3.h"

/// @brief 球
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