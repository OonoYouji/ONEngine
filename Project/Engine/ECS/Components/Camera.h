#pragma once

#include "Engine/Core/Math/Math.h"

namespace Engine::ECS {

///
/// カメラコンポーネント
///
struct Camera {
	float fov = 45.0f;
	float nearZ = 0.1f;
	float farZ = 1000.0f;
	
	// 計算済みの行列を保持することも可能
	// Engine::Math::Matrix4x4 viewMatrix;
	// Engine::Math::Matrix4x4 projectionMatrix;
};

} // namespace Engine::ECS
