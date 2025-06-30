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


	/// @brief Rayと箱の当たり判定を取る
	/// @param _rayStartPosition rayの始点
	/// @param _rayDirection rayのベクトル
	/// @param _cubePosition cubeの中心点
	/// @param _cubeSize cubeのサイズ
	/// @return true: 衝突している false: 衝突していない
	bool RayVsCube(
		const Vector3& _rayStartPosition, const Vector3& _rayDirection,
		const Vector3& _cubePosition, const Vector3& _cubeSize
	);

	bool CubeVsCube(
		const Vector3& _cube1Position, const Vector3& _cube1Size,
		const Vector3& _cube2Position, const Vector3& _cube2Size
	);

	bool CubeVsSphere(
		const Vector3& _cubePosition, const Vector3& _cubeSize,
		const Vector3& _sphereCenter, float _sphereRadius
	);

	bool CubeVsCapsule(
		const Vector3& _cubePosition, const Vector3& _cubeSize,
		const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius
	);


	bool SphereVsSphere(
		const Vector3& _sphere1Center, float _sphere1Radius,
		const Vector3& _sphere2Center, float _sphere2Radius
	);

	bool SphereVsCapsule(
		const Vector3& _sphereCenter, float _sphereRadius,
		const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius
	);


}



namespace CollisionMath {

	Vec3 ClosestPointOnAABB(const Vector3& point, const Vector3& _aabbMin, const Vector3& _aabbMax);

	void ClosestPointsSegmentAABB(
		const Vector3& _lineStart, const Vector3& _lineEnd,
		const Vector3& _aabbMin, const Vector3& _aabbMax,
		Vector3& _outSegmentPoint, Vector3& _outAABBPoint
	);

}