#pragma once

#include <BaseCamera.h>

#include <Vector3.h>

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