#pragma once

#include "Engine/Core/Math/Math.h"

namespace Engine::ECS {

///
/// トランスフォームコンポーネント
///
struct Transform {
	Engine::Math::Vector3 position = { 0.0f, 0.0f, 0.0f };
	Engine::Math::Vector3 rotation = { 0.0f, 0.0f, 0.0f };
	Engine::Math::Vector3 scale    = { 1.0f, 1.0f, 1.0f };
};

} // namespace Engine::ECS
