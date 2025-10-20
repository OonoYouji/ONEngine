#pragma once

/// engine
#include "Engine/Core/Utility/Math/Matrix4x4.h"

/// ===================================================
/// カメラのビュー行列と射影行列
/// ===================================================
struct ViewProjection {
	Matrix4x4 matVP;         ///< ビュープロジェクション行列
	Matrix4x4 matView;       ///< ビュー行列
	Matrix4x4 matProjection; ///< プロジェクション行列
};

