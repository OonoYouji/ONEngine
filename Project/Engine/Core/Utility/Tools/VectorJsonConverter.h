#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

void from_json(const nlohmann::json& j, Vec2& v);
void to_json(nlohmann::json& j, const Vec2& v);

void from_json(const nlohmann::json& j, Vec3& v);
void to_json(nlohmann::json& j, const Vec3& v);

void from_json(const nlohmann::json& j, Vec4& v);
void to_json(nlohmann::json& j, const Vec4& v);
