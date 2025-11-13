#pragma once

/// engine
#include "Engine/Core/Utility/Math/Vector2.h"

/// /////////////////////////////////////////////////
/// @brief UV変形用データ構造体
/// /////////////////////////////////////////////////
struct UVTransform {
	Vector2 position = Vector2::kZero; /// オフセット
	Vector2 scale = Vector2::kOne; /// スケール
	float   rotate = 0.0f;   /// 回転
	float   pad1[3];
};


/// uv transform
void to_json(nlohmann::json& _j, const UVTransform& _uvTransform);
void from_json(const nlohmann::json& _j, UVTransform& _uvTransform);
