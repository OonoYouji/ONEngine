#pragma once

/// std
#include <array>

/// engine
#include "Engine/Core/Utility/Math/Vector3.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// CollisionCheck 
/// ///////////////////////////////////////////////////
namespace CollisionCheck {

/**
 * @brief 線分と球の衝突判定を行います。
 * @param _lineStart 線分の始点
 * @param _lineEnd 線分の終点
 * @param _sphereCenter 球の中心座標
 * @param _sphereRadius 球の半径
 * @return 衝突している場合は true、していない場合は false
 */
bool LineVsSphere(
	const Vector3& _lineStart, const Vector3& _lineEnd,
	const Vector3& _sphereCenter, float _sphereRadius
);

/**
 * @brief 線分と三角形の衝突判定を行います。
 * @param _lineStart 線分の始点
 * @param _lineEnd 線分の終点
 * @param _triangleVertices 三角形を構成する3頂点座標の配列
 * @return 衝突している場合は true、していない場合は false
 */
bool LineVsTriangle(
	const Vector3& _lineStart, const Vector3& _lineEnd,
	const std::array<Vector3, 3>& _triangleVertices
);

/**
 * @brief レイ（半直線）と球の衝突判定を行います。
 * @param _rayStartPosition レイの始点座標
 * @param _rayDirection レイの方向ベクトル（正規化済みであること）
 * @param _sphereCenter 球の中心座標
 * @param _sphereRadius 球の半径
 * @return 衝突している場合は true、していない場合は false
 */
bool RayVsSphere(
	const Vector3& _rayStartPosition, const Vector3& _rayDirection,
	const Vector3& _sphereCenter, float _sphereRadius
);


/**
 * @brief レイ（半直線）と箱（AABB）の衝突判定を行います。
 * @param _rayStartPosition レイの始点座標
 * @param _rayDirection レイの方向ベクトル
 * @param _cubePosition 箱の中心座標
 * @param _cubeSize 箱の各軸サイズ
 * @return 衝突している場合は true、していない場合は false
 */
bool RayVsCube(
	const Vector3& _rayStartPosition, const Vector3& _rayDirection,
	const Vector3& _cubePosition, const Vector3& _cubeSize
);

/**
 * @brief 箱（AABB）同士の衝突判定を行います。衝突法線と貫入深さの算出もサポートします。
 * @param _cube1Position 箱1の中心座標
 * @param _cube1Size 箱1のサイズ
 * @param _cube2Position 箱2の中心座標
 * @param _cube2Size 箱2のサイズ
 * @param _outNormal [out] 衝突時の押し戻し用法線ベクトルの出力先ポインタ（不要ならnullptr）
 * @param _outPenetration [out] 衝突時の重なり深さ（貫入量）の出力先ポインタ（不要ならnullptr）
 * @return 衝突している場合は true、していない場合は false
 */
bool CubeVsCube(
	const Vector3& _cube1Position, const Vector3& _cube1Size,
	const Vector3& _cube2Position, const Vector3& _cube2Size,
	Vector3* _outNormal = nullptr,
	float* _outPenetration = nullptr
);

/**
 * @brief 箱（AABB）と球の衝突判定を行います。
 * @param _cubePosition 箱の中心座標
 * @param _cubeSize 箱のサイズ
 * @param _sphereCenter 球の中心座標
 * @param _sphereRadius 球の半径
 * @param _outClosestPoint [out] AABB上で球の中心に最も近い近接座標の出力先ポインタ（不要ならnullptr）
 * @param _outDistance [out] 最も近い点と球の中心との距離の出力先ポインタ（不要ならnullptr）
 * @return 衝突している場合は true、していない場合は false
 */
bool CubeVsSphere(
	const Vector3& _cubePosition, const Vector3& _cubeSize,
	const Vector3& _sphereCenter, float _sphereRadius,
	Vector3* _outClosestPoint = nullptr,
	float* _outDistance = nullptr
);

/**
 * @brief 箱（AABB）とカプセル形状の衝突判定を行います。
 * @param _cubePosition 箱の中心座標
 * @param _cubeSize 箱のサイズ
 * @param _capsuleStart カプセル線分の始点座標
 * @param _capsuleEnd カプセル線分の終点座標
 * @param _capsuleRadius カプセルの半径
 * @return 衝突している場合は true、していない場合は false
 */
bool CubeVsCapsule(
	const Vector3& _cubePosition, const Vector3& _cubeSize,
	const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius
);

/**
 * @brief 球同士の衝突判定を行います。
 * @param _sphere1Center 球1の中心座標
 * @param _sphere1Radius 球1の半径
 * @param _sphere2Center 球2の中心座標
 * @param _sphere2Radius 球2の半径
 * @return 衝突している場合は true、していない場合は false
 */
bool SphereVsSphere(
	const Vector3& _sphere1Center, float _sphere1Radius,
	const Vector3& _sphere2Center, float _sphere2Radius
);

/**
 * @brief 球とカプセル形状の衝突判定を行います。
 * @param _sphereCenter 球の中心座標
 * @param _sphereRadius 球の半径
 * @param _capsuleStart カプセル線分の始点座標
 * @param _capsuleEnd カプセル線分の終点座標
 * @param _capsuleRadius カプセルの半径
 * @return 衝突している場合は true、していない場合は false
 */
bool SphereVsCapsule(
	const Vector3& _sphereCenter, float _sphereRadius,
	const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius
);


}



namespace CollisionMath {

/**
 * @brief 任意の座標点から、指定されたAABB（最小/最大境界）上で最も近接する点を算出します。
 * @param _point 入力座標
 * @param _aabbMin AABBの最小座標（X, Y, Z）
 * @param _aabbMax AABBの最大座標（X, Y, Z）
 * @return _pointに最も近いAABB上の点
 */
Vector3 ClosestPointOnAABB(const Vector3& _point, const Vector3& _aabbMin, const Vector3& _aabbMax);

/**
 * @brief 線分とAABBの最近接点をそれぞれ求めます。
 * @param _lineStart 線分の始点
 * @param _lineEnd 線分の終点
 * @param _aabbMin AABBの最小座標
 * @param _aabbMax AABBの最大座標
 * @param _outSegmentPoint [out] 線分上の最近接点
 * @param _outAABBPoint [out] AABB上の最近接点
 */
void ClosestPointsSegmentAABB(
	const Vector3& _lineStart, const Vector3& _lineEnd,
	const Vector3& _aabbMin, const Vector3& _aabbMax,
	Vector3& _outSegmentPoint, Vector3& _outAABBPoint
);

}

} /// namespace ONEngine