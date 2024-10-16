#pragma once

#include "Objects/Camera/Manager/BaseCamera.h"

#include "Math/Vector2.h"

/// ===================================================
/// デバッグ用カメラ
/// ===================================================
class DebugCamera final : public BaseCamera {
public:

	DebugCamera();
	~DebugCamera();

	void Initialize() override;
	void Update() override;

private:

	Vec3 velocity_{};

};