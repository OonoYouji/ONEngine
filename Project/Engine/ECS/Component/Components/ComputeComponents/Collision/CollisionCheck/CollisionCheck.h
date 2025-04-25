#pragma once

/// engine
#include "Engine/Core/Utility/Utility.h"

/// ///////////////////////////////////////////////////
/// CollisionCheck 
/// ///////////////////////////////////////////////////
namespace CollisionCheck {

	/// @brief 線分と球の当たり判定を取る
	/// @param _lineStart 線分の始点
	/// @param _lineEnd 線分の終点
	/// @param _sphereCenter 球の中心
	/// @param _sphereRadius 球の半径
	/// @return true: 衝突している false: 衝突していない
	bool LineVsSphere(
		const Vector3& _lineStart, const Vector3& _lineEnd,
		const Vector3& _sphereCenter, float _sphereRadius
	);

	/// @brief 線と三角形の当たり判定を取る
	/// @param _lineStart 線の始点
	/// @param _lineEnd 線の終点
	/// @param _triangleVertices 三角形の3頂点 
	/// @return true: 衝突している false: 衝突していない
	bool LineVsTriangle(
		const Vector3& _lineStart, const Vector3& _lineEnd,
		const std::array<Vector3, 3>& _triangleVertices
	);

	/// @brief Rayと球の当たり判定を取る
	/// @param _rayDirection Rayの方向ベクトル
	/// @param _sphereCenter 球の中心
	/// @param _sphereRadius 球の半径
	/// @return true: 衝突している false: 衝突していない
	bool RayVsSphere(
		const Vector3& _rayStartPosition, const Vector3& _rayDirection,
		const Vector3& _sphereCenter, float _sphereRadius
	);


}

