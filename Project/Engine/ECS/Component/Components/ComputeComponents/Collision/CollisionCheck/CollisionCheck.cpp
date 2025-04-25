#include "CollisionCheck.h"

bool CollisionCheck::LineVsSphere(const Vector3& _lineStart, const Vector3& _lineEnd, const Vector3& _sphereCenter, float _sphereRadius) {

	Vector3 lineDiff = _lineEnd - _lineStart;


	return false;
}

bool CollisionCheck::LineVsTriangle(const Vector3& _lineStart, const Vector3& _lineEnd, const std::array<Vector3, 3>& _triangleVertices) {

	Vector3&& lineDiff = _lineEnd - _lineStart;
	Vector3&& v01 = _triangleVertices[1] - _triangleVertices[0];
	Vector3&& v12 = _triangleVertices[2] - _triangleVertices[1];
	Vector3&& v20 = _triangleVertices[0] - _triangleVertices[2];

	Vector3&& normal = Vector3::Cross(v01, v12);
	float distance = Vector3::Dot(
		(_triangleVertices[0] + _triangleVertices[1] + _triangleVertices[2]) / 3.0f, /// 3頂点の平均が中心
		normal
	);

	float dot = Vector3::Dot(normal, lineDiff);
	if (dot == 0.0f) { return false; }
	float t = (distance - Vector3::Dot(normal, _lineStart)) / dot;
	Vector3&& planePoint = _lineStart + (lineDiff * t);

	Vector3&& cross01 = Vector3::Cross(v01, planePoint - _triangleVertices[1]);
	Vector3&& cross12 = Vector3::Cross(v12, planePoint - _triangleVertices[2]);
	Vector3&& cross20 = Vector3::Cross(v20, planePoint - _triangleVertices[0]);

	if (Vector3::Dot(cross01, normal) >= 0.0f
		&& Vector3::Dot(cross12, normal) >= 0.0f
		&& Vector3::Dot(cross20, normal) >= 0.0f) {
		return true;
	}

	return false;
}

bool CollisionCheck::RayVsSphere(const Vector3& _rayStartPosition, const Vector3& _rayDirection, const Vector3& _sphereCenter, float _sphereRadius) {

	Vector3&& rayDirection = _rayDirection.Normalize();
	Vector3&& sphereToRay = _sphereCenter - rayDirection + _rayStartPosition;

	/// 最近接点を計算
	float dot = Vector3::Dot(sphereToRay, rayDirection);
	Vector3&& nearPos = rayDirection * dot;

	/// 球の中心からRayの最近接点までの距離を計算
	float distance = Vector3::Length(sphereToRay - nearPos);
	return distance <= _sphereRadius;
}
