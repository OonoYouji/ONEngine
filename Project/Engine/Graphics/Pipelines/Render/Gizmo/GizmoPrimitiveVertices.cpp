#include "GizmoPrimitiveVertices.h"
#include <cmath>
#include <numbers>

#include "Engine/Core/Utility/Math/Matrix4x4.h"

using namespace ONEngine;
using namespace GizmoPrimitive;

namespace {
	/// @brief 1つの線分から6頂点（厚みのある矩形）を生成する
	void AddThickLineSegment(std::vector<VertexData>& _out, const Vector3& _v0, const Vector3& _v1, const Vector4& _color, float _thickness) {
		Vector4 p0 = Math::ConvertToVector4(_v0, 1.0f);
		Vector4 p1 = Math::ConvertToVector4(_v1, 1.0f);

		// expansionDir: x=側(-1 or 1), y=端点(0=start, 1=end)
		// 頂点の配置(始点から終点へ向かって、左=L, 右=R)
		// p0L(0) -- p1L(2)
		//   |    \    |
		// p0R(1) -- p1R(3)
		
		VertexData v[4];
		for (int i = 0; i < 4; ++i) {
			v[i].position = p0;      // 全ての頂点に共通の「始点」
			v[i].otherPosition = p1; // 全ての頂点に共通の「終点」
			v[i].color = _color;
			v[i].thickness = _thickness;
		}

		// expansionDir.x : 法線方向へのオフセット (-1 or 1)
		// expansionDir.y : 始点なら0, 終点なら1（シェーダーでどちらを使うか選択）
		v[0].expansionDir = Vector2(-1.0f, 0.0f); // 始点・左
		v[1].expansionDir = Vector2(1.0f, 0.0f);  // 始点・右
		v[2].expansionDir = Vector2(-1.0f, 1.0f); // 終点・左
		v[3].expansionDir = Vector2(1.0f, 1.0f);  // 終点・右

		// CW(時計回り)で2つの三角形を生成
		// Tri 1: (0, 2, 1) -> 始点左, 終点左, 始点右
		_out.push_back(v[0]); _out.push_back(v[2]); _out.push_back(v[1]);
		// Tri 2: (1, 2, 3) -> 始点右, 終点左, 終点右
		_out.push_back(v[1]); _out.push_back(v[2]); _out.push_back(v[3]);
	}
}

std::vector<VertexData> ONEngine::GetSphereVertices(const Vector3& _center, float _radius, const Vector4& _color, float _thickness, size_t _segment) {
	const float deltaAngle = 2.0f * std::numbers::pi_v<float> / (float)_segment;
	std::vector<VertexData> outVertices;

	auto addCircle = [&](const Vector3& _axis1, const Vector3& _axis2) {
		for (size_t i = 0; i < _segment; ++i) {
			float angle0 = (float)i * deltaAngle;
			float angle1 = (float)(i + 1) * deltaAngle;

			Vector3 dir0 = Vector3::Normalize(_axis1 * std::cos(angle0) + _axis2 * std::sin(angle0));
			Vector3 dir1 = Vector3::Normalize(_axis1 * std::cos(angle1) + _axis2 * std::sin(angle1));

			AddThickLineSegment(outVertices, _center + dir0 * _radius, _center + dir1 * _radius, _color, _thickness);
		}
	};

	addCircle(Vector3::Right, Vector3::Up);
	addCircle(Vector3::Up, Vector3::Forward);
	addCircle(Vector3::Forward, Vector3::Right);

	return outVertices;
}

std::vector<VertexData> ONEngine::GetCubeVertices(const Vector3& _center, const Vector3& _size, const Quaternion& _rotate, const Vector4& _color, float _thickness) {
	Vector3 halfSize = _size * 0.5f;
	std::vector<VertexData> outVertices;

	// 回転行列の作成
	Matrix4x4 rotateMat = Matrix4x4::MakeRotate(_rotate);

	Vector3 baseVertices[8] = {
		Vector3(-halfSize.x, -halfSize.y, -halfSize.z),
		Vector3(halfSize.x, -halfSize.y, -halfSize.z),
		Vector3(halfSize.x, halfSize.y, -halfSize.z),
		Vector3(-halfSize.x, halfSize.y, -halfSize.z),
		Vector3(-halfSize.x, -halfSize.y, halfSize.z),
		Vector3(halfSize.x, -halfSize.y, halfSize.z),
		Vector3(halfSize.x, halfSize.y, halfSize.z),
		Vector3(-halfSize.x, halfSize.y, halfSize.z)
	};

	Vector3 vertices[8];
	for (int i = 0; i < 8; i++) {
		// 回転を適用してから中心座標を足す
		vertices[i] = _center + Matrix4x4::Transform(baseVertices[i], rotateMat);
	}

	int32_t indices[] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	for (size_t i = 0; i < sizeof(indices) / sizeof(int); i += 2) {
		AddThickLineSegment(outVertices, vertices[indices[i]], vertices[indices[i + 1]], _color, _thickness);
	}

	return outVertices;
}

std::vector<GizmoPrimitive::VertexData> ONEngine::GetRectVertices(const Matrix4x4& _matWorld, const Vector4& _color, float _thickness, const Vector2& _rectSize) {
	std::vector<GizmoPrimitive::VertexData> outVertices;

	Vector3 vertices[4] = {
		Vector3(-_rectSize.x, 0.0f, -_rectSize.y),
		Vector3(+_rectSize.x, 0.0f, -_rectSize.y),
		Vector3(+_rectSize.x, 0.0f, +_rectSize.y),
		Vector3(-_rectSize.x, 0.0f, +_rectSize.y)
	};

	for (int i = 0; i < 4; ++i) {
		vertices[i] = Matrix4x4::Transform(vertices[i], _matWorld);
	}

	for (int i = 0; i < 4; ++i) {
		AddThickLineSegment(outVertices, vertices[i], vertices[(i + 1) % 4], _color, _thickness);
	}

	return outVertices;
}

std::vector<VertexData> ONEngine::GetLineVertices(const Vector3& _v0, const Vector3& _v1, const Vector4& _color, float _thickness) {
	std::vector<VertexData> outVertices;
	AddThickLineSegment(outVertices, _v0, _v1, _color, _thickness);
	return outVertices;
}
