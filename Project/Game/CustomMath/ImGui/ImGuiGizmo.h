#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"

void ImGuiGizmo(
	Vec3* _position, Vec3* _rotate, Vec3* _scale,
	ROTATE_ORDER _rotateOrder
);

void ImGuiGizmo(
	Mat4* _matTransform,
	Vec3* _position, Vec3* _rotate, Vec3* _scale,
	ROTATE_ORDER _rotateOrder
);


