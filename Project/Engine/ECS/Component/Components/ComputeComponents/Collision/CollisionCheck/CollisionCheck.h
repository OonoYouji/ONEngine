#pragma once

/// std
#include <array>

/// engine
#include "Engine/Core/Utility/Math/Vector3.h"

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

	/// @brief AABB同士の当たり判定を取る
	/// @param _cube1Position 一つ目のCubeの中心点
	/// @param _cube1Size 一つ目のCubeのサイズ
	/// @param _cube2Position 二つ目のCubeの中心点
	/// @param _cube2Size 二つ目のCubeのサイズ
	/// @return true: 衝突している false: 衝突していない
	bool CubeVsCube(
		const Vector3& _cube1Position, const Vector3& _cube1Size,
		const Vector3& _cube2Position, const Vector3& _cube2Size
	);

	/// @brief AABBと球の当たり判定を取る
	/// @param _cubePosition AABBの中心点
	/// @param _cubeSize AABBのサイズ
	/// @param _sphereCenter Sphereの中心点
	/// @param _sphereRadius Sphereの半径
	/// @return true: 衝突している false: 衝突していない
	bool CubeVsSphere(
		const Vector3& _cubePosition, const Vector3& _cubeSize,
		const Vector3& _sphereCenter, float _sphereRadius
	);

	/// @brief AABBとカプセルの当たり判定を取る
	/// @param _cubePosition AABBの中心点
	/// @param _cubeSize AABBのサイズ
	/// @param _capsuleStart Capsuleの始点
	/// @param _capsuleEnd Capsuleの終点
	/// @param _capsuleRadius Capsuleの半径
	/// @return true: 衝突している false: 衝突していない
	bool CubeVsCapsule(
		const Vector3& _cubePosition, const Vector3& _cubeSize,
		const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius
	);

	/// @brief Sphere同士の当たり判定を取る
	/// @param _sphere1Center Sphere1の中心
	/// @param _sphere1Radius Sphere1の半径
	/// @param _sphere2Center Sphere2の中心
	/// @param _sphere2Radius Sphere2の半径
	/// @return true: 衝突している false: 衝突していない
	bool SphereVsSphere(
		const Vector3& _sphere1Center, float _sphere1Radius,
		const Vector3& _sphere2Center, float _sphere2Radius
	);

	/// @brief SphereとCapsuleの当たり判定を取る
	/// @param _sphereCenter Sphereの中心
	/// @param _sphereRadius Sphereの半径
	/// @param _capsuleStart Capsuleの始点
	/// @param _capsuleEnd Capsuleの終点
	/// @param _capsuleRadius Capsuleの半径
	/// @return true: 衝突している false: 衝突していない
	bool SphereVsCapsule(
		const Vector3& _sphereCenter, float _sphereRadius,
		const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius
	);


}



namespace CollisionMath {

	/// @brief _pointに最も近いAABB上の点を求める
	/// @param _point 
	/// @param _aabbMin AABBの最小点
	/// @param _aabbMax AABBの最大点
	/// @return _pointに最も近いAABB上の点
	Vector3 ClosestPointOnAABB(const Vector3& _point, const Vector3& _aabbMin, const Vector3& _aabbMax);

	/// @brief LineとAABBの最近接点を求める
	/// @param _lineStart Lineの始点
	/// @param _lineEnd Lineの終点
	/// @param _aabbMin AABBの最小点
	/// @param _aabbMax AABBの最大点
	/// @param _outSegmentPoint Segment上の最近接点
	/// @param _outAABBPoint AABB上の最近接点
	void ClosestPointsSegmentAABB(
		const Vector3& _lineStart, const Vector3& _lineEnd,
		const Vector3& _aabbMin, const Vector3& _aabbMax,
		Vector3& _outSegmentPoint, Vector3& _outAABBPoint
	);

}