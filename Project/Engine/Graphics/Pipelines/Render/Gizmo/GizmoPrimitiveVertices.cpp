#include "GizmoPrimitiveVertices.h"


using namespace ONEngine;
using namespace GizmoPrimitive;

std::vector<VertexData> ONEngine::GetSphereVertices(const Vector3& _center, float _radius, const Vector4& _color, size_t _segment) {
	/// ----- 球を構成する頂点を計算 ----- ///

	const float deltaAngle = 2.0f * std::numbers::pi_v<float> / _segment;
	std::vector<VertexData> outVertices;

	/// ----- 指定した2つの軸から円を描くラムダ式 ----- ///
	auto addCircle = [&](const Vector3& _axis1, const Vector3& _axis2) {
		for (int i = 0; i < _segment; ++i) {
			float angle0 = i * deltaAngle;
			float angle1 = (i + 1) * deltaAngle;

			Vector3 dir0 = Vector3::Normalize(_axis1 * std::cos(angle0) + _axis2 * std::sin(angle0));
			Vector3 dir1 = Vector3::Normalize(_axis1 * std::cos(angle1) + _axis2 * std::sin(angle1));

			VertexData v0;
			v0.position = Vector4(Mathf::ConvertToVector4(_center + dir0 * _radius, 1.0f));
			v0.color = _color;

			VertexData v1;
			v1.position = Vector4(Mathf::ConvertToVector4(_center + dir1 * _radius, 1.0f));
			v1.color = _color;

			outVertices.push_back(v0);
			outVertices.push_back(v1);
		}
		};

	// XY平面
	addCircle(Vector3::Right, Vector3::Up);
	// YZ平面
	addCircle(Vector3::Up, Vector3::Forward);
	// ZX平面
	addCircle(Vector3::Forward, Vector3::Right);

	return outVertices;
}

std::vector<VertexData> ONEngine::GetCubeVertices(const Vector3& _center, const Vector3& _size, const Vector4& _color) {
	/// ----- 立方体を構成する頂点を計算 ----- ///

	Vector3 halfSize = _size * 0.5f;
	std::vector<VertexData> outVertices;

	// 立方体の8頂点
	Vector3 vertices[8] = {
		_center + Vector3(-halfSize.x, -halfSize.y, -halfSize.z), // 0
		_center + Vector3(halfSize.x, -halfSize.y, -halfSize.z),  // 1
		_center + Vector3(halfSize.x, halfSize.y, -halfSize.z),   // 2
		_center + Vector3(-halfSize.x, halfSize.y, -halfSize.z),  // 3
		_center + Vector3(-halfSize.x, -halfSize.y, halfSize.z),  // 4
		_center + Vector3(halfSize.x, -halfSize.y, halfSize.z),   // 5
		_center + Vector3(halfSize.x, halfSize.y, halfSize.z),    // 6
		_center + Vector3(-halfSize.x, halfSize.y, halfSize.z)    // 7
	};

	// 線を引く頂点のペアリスト
	int32_t indices[] = {
		0, 1, 1, 2, 2, 3, 3, 0, // 底面
		4, 5, 5, 6, 6, 7, 7, 4, // 上面
		0, 4, 1, 5, 2, 6, 3, 7  // 側面
	};

	VertexData v0, v1;
	for (size_t i = 0; i < sizeof(indices) / sizeof(int); i += 2) {
		v0.position = Vector4(Mathf::ConvertToVector4(vertices[indices[i + 0]], 1.0f));
		v0.color = _color;

		v1.position = Vector4(Mathf::ConvertToVector4(vertices[indices[i + 1]], 1.0f));
		v1.color = _color;

		outVertices.push_back(v0);
		outVertices.push_back(v1);
	}

	return outVertices;
}

std::vector<GizmoPrimitive::VertexData> ONEngine::GetRectVertices(const Matrix4x4& _matWorld, const Vector4& _color, const Vector2& _rectSize) {
	/// ----- 2D矩形を構成する頂点を計算(_matWorld次第で回転、拡縮する) ----- ///

	std::vector<GizmoPrimitive::VertexData> outVertices;

	/// 2Dの矩形の4頂点を計算
	Vector3 vertices[4] = {
		Vector3(-_rectSize.x, 0.0f, -_rectSize.y), // 左下
		Vector3(+_rectSize.x, 0.0f, -_rectSize.y), // 右下
		Vector3(+_rectSize.x, 0.0f, +_rectSize.y), // 右上
		Vector3(-_rectSize.x, 0.0f, +_rectSize.y)  // 左上
	};

	/// マトリックス変換を適用
	for (int i = 0; i < 4; ++i) {
		vertices[i] = Matrix4x4::Transform(vertices[i], _matWorld);
	}

	/// 各頂点をつなぐ線を生成
	for (int i = 0; i < 4; ++i) {
		VertexData v0, v1;
		v0.position = Vector4(Mathf::ConvertToVector4(vertices[i], 1.0f));
		v0.color = _color;
		v1.position = Vector4(Mathf::ConvertToVector4(vertices[(i + 1) % 4], 1.0f));
		v1.color = _color;
		outVertices.push_back(v0);
		outVertices.push_back(v1);
	}

	return outVertices;
}
