#pragma once

/// std
#include <vector>

/// engine
#include "Engine/Core/Utility/Utility.h"

namespace GizmoPrimitive {
	struct VertexData {
		Vector4 position;
		Vector4 color;
	};
}


std::vector<GizmoPrimitive::VertexData> GetSphereVertices(const Vector3& _center, float _radius, const Vector4& _color, size_t _segment = 24);
std::vector<GizmoPrimitive::VertexData> GetCubeVertices(const Vector3& _center, const Vector3& _size, const Vector4& _color);
std::vector<GizmoPrimitive::VertexData> GetRectVertices(const Matrix4x4& _matWorld, const Vector4& _color, const Vector2& _rectSize = Vector2::kOne);