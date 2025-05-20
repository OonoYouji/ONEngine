#pragma once

/// engine
#include "Camera.h"

/// ///////////////////////////////////////////////////
/// デバッグカメラ
/// ///////////////////////////////////////////////////
class DebugCamera : public Camera {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DebugCamera(class DxDevice* _dxDevice);
	~DebugCamera();

	void Initialize() override;
	void Update() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	bool active_;

	bool isMoving_; ///< 移動中か

};

