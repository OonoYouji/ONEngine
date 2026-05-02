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


/// @brief 円錐
struct Cone {
	Vector3 center;
	float angle;  // 円錐の角度
	float radius; // 円の半径
	float height; // 円錐の高さ
};

/// ///////////////////////////////////////////////////
/// 以下より上の構造体のJson変換関数
/// ///////////////////////////////////////////////////

void from_json(const nlohmann::json& _j, Sphere& _s);
void to_json(nlohmann::json& _j, const Sphere& _s);

void from_json(const nlohmann::json& _j, Cube& _c);
void to_json(nlohmann::json& _j, const Cube& _c);

void from_json(const nlohmann::json& _j, Line& _l);
void to_json(nlohmann::json& _j, const Line& _l);

void from_json(const nlohmann::json& _j, Ray& _r);
void to_json(nlohmann::json& _j, const Ray& _r);

void from_json(const nlohmann::json& _j, Plane& _p);
void to_json(nlohmann::json& _j, const Plane& _p);

void from_json(const nlohmann::json& _j, Frustum& _f);
void to_json(nlohmann::json& _j, const Frustum& _f);

void from_json(const nlohmann::json& _j, Cone& _c);
void to_json(nlohmann::json& _j, const Cone& _c);

} /// ONEngine
