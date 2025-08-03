#pragma once

/// engine
#include "../../Interface/IEntity.h"

/// ///////////////////////////////////////////////////
/// デバッグカメラ
/// ///////////////////////////////////////////////////
class DebugCamera : public IEntity {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DebugCamera();
	~DebugCamera();

	void Initialize() override;
	void Update() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	bool isActive_;
	bool isMoving_; ///< 移動中か

	float moveSpeed_;
	Vector3 eulerAngles_; ///< カメラの回転角度

};

