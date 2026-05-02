#pragma once

#include "Engine/Core/Math/Math.h"

namespace Engine::ECS {

///
/// 平行光源コンポーネント
///
struct DirectionalLight {
	Engine::Math::Vector3 color = { 1.0f, 1.0f, 1.0f };
	float intensity = 1.0f;
	Engine::Math::Vector3 direction = { 0.0f, -1.0f, 1.0f };
};

///
/// 点光源コンポーネント
///
struct PointLight {
	Engine::Math::Vector3 color = { 1.0f, 1.0f, 1.0f };
	float intensity = 1.0f;
	float radius = 10.0f;
};

} // namespace Engine::ECS
