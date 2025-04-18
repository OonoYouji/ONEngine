#pragma once

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

void from_json(const nlohmann::json& _j, Vec2& _v);
void to_json(nlohmann::json& _j, const Vec2& _v);

void from_json(const nlohmann::json& _j, Vec3& _v);
void to_json(nlohmann::json& _j, const Vec3& _v);

void from_json(const nlohmann::json& _j, Vec4& _v);
void to_json(nlohmann::json& _j, const Vec4& _v);
