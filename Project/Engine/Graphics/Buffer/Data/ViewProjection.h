#pragma once

/// engine
#include "Engine/Utility/Math/Matrix4x4.h"

/// ===================================================
/// カメラのビュー行列と射影行列
/// ===================================================
struct ViewProjection {
	Matrix4x4 view;       ///< ビュー行列
	Matrix4x4 projection; ///< 射影行列
	Matrix4x4 vp;         ///< ビュープロジェクション行列
};

