#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Core/Utility/Utility.h"


namespace ONEngine {

/// @brief Gizmo用のプリミティブ頂点データ
namespace GizmoPrimitive {

struct VertexData {
	Vector4 position;
	Vector4 color;
};
}

/// @brief Sphereの頂点データを取得する
/// @param _center Sphereの中心
/// @param _radius Sphereの半径
/// @param _color Sphereの色
/// @param _segment 線の分割数
/// @return 引数で指定したSphereの頂点データ
std::vector<GizmoPrimitive::VertexData> GetSphereVertices(const Vector3& _center, float _radius, const Vector4& _color, size_t _segment = 24);

/// @brief Cubeの頂点データを取得する
/// @param _center Cubeの中心
/// @param _size Cubeのサイズ
/// @param _color Cubeの色
/// @return 引数で指定したCubeの頂点データ
std::vector<GizmoPrimitive::VertexData> GetCubeVertices(const Vector3& _center, const Vector3& _size, const Vector4& _color);

/// @brief 矩形の頂点データを取得する
/// @param _matWorld ワールド座標
/// @param _color 色
/// @param _rectSize 矩形の大きさ
/// @return 矩形の頂点データ
std::vector<GizmoPrimitive::VertexData> GetRectVertices(const Matrix4x4& _matWorld, const Vector4& _color, const Vector2& _rectSize = Vector2::kOne);

} /// ONEngine