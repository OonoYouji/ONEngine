#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/Utility/Math/Quaternion.h"


namespace ONEngine {

/// @brief Gizmo用のプリミティブ頂点データ
namespace GizmoPrimitive {

struct VertexData {
	Vector4 position;      ///< 自身の位置 (start or end)
	Vector4 otherPosition; ///< もう一方の端点
	Vector4 color;         ///< 頂点の色
	float thickness;       ///< 線の太さ (pixel)
	Vector2 expansionDir;  ///< 拡大方向 (-1 or 1)
};
}

/// @brief Sphereの頂点データを取得する
/// @param _center Sphereの中心
/// @param _radius Sphereの半径
/// @param _color Sphereの色
/// @param _thickness 線の太さ
/// @param _segment 線の分割数
/// @return 1セグメントあたり6頂点(TRIANGLELIST)の頂点データ
std::vector<GizmoPrimitive::VertexData> GetSphereVertices(const Vector3& _center, float _radius, const Vector4& _color, float _thickness = 1.0f, size_t _segment = 24);

/// @brief Cubeの頂点データを取得する
/// @param _center Cubeの中心
/// @param _size Cubeのサイズ
/// @param _rotate Cubeの回転
/// @param _color Cubeの色
/// @param _thickness 線の太さ
/// @return 1セグメントあたり6頂点(TRIANGLELIST)の頂点データ
std::vector<GizmoPrimitive::VertexData> GetCubeVertices(const Vector3& _center, const Vector3& _size, const Quaternion& _rotate, const Vector4& _color, float _thickness = 1.0f);

/// @brief 矩形の頂点データを取得する
/// @param _matWorld ワールド座標
/// @param _color 色
/// @param _thickness 線の太さ
/// @param _rectSize 矩形の大きさ
/// @return 1セグメントあたり6頂点(TRIANGLELIST)の頂点データ
std::vector<GizmoPrimitive::VertexData> GetRectVertices(const Matrix4x4& _matWorld, const Vector4& _color, float _thickness = 1.0f, const Vector2& _rectSize = Vector2::One);

/// @brief 線分の頂点データを取得する
/// @param _v0 始点
/// @param _v1 終点
/// @param _color 色
/// @param _thickness 太さ
/// @return 1セグメントあたり6頂点(TRIANGLELIST)の頂点データ
std::vector<GizmoPrimitive::VertexData> GetLineVertices(const Vector3& _v0, const Vector3& _v1, const Vector4& _color, float _thickness = 1.0f);

} /// ONEngine