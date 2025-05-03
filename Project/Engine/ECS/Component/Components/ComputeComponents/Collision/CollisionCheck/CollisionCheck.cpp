#define NOMINMAX
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

bool CollisionCheck::RayVsCube(const Vector3& _rayStartPosition, const Vector3& _rayDirection, const Vector3& _cubePosition, const Vector3& _cubeSize) {
	Vector3 aabbMin = _cubePosition - _cubeSize / 2.0f;
	Vector3 aabbMax = _cubePosition + _cubeSize / 2.0f;

	Vector3 min = (aabbMin - _rayStartPosition) / _rayDirection;
	Vector3 max = (aabbMax - _rayStartPosition) / _rayDirection;

	Vector3 nearPoint = {
		std::min(min.x, max.x),
		std::min(min.y, max.y),
		std::min(min.z, max.z)
	};

	Vector3 farPoint = {
		std::max(min.x, max.x),
		std::max(min.y, max.y),
		std::max(min.z, max.z)
	};

	float tmin = std::max({ nearPoint.x, nearPoint.y, nearPoint.z });
	float tmax = std::min({ farPoint.x, farPoint.y, farPoint.z });

	/// Ray用の制限
	if (tmax < 0.0f) {
		return false;
	}

	if (tmin <= tmax) {
		return true;
	}

	return false;
}

bool CollisionCheck::CubeVsSphere(const Vector3& _cubePosition, const Vector3& _cubeSize, const Vector3& _sphereCenter, float _sphereRadius) {

	Vector3&& cubeMin = _cubePosition - _cubeSize / 2.0f;
	Vector3&& cubeMax = _cubePosition + _cubeSize / 2.0f;

	Vector3&& colsestPoint = {
		std::clamp(_sphereCenter.x, cubeMin.x, cubeMax.x),
		std::clamp(_sphereCenter.y, cubeMin.y, cubeMax.y),
		std::clamp(_sphereCenter.z, cubeMin.z, cubeMax.z)
	};

	float distance = Vector3::Length(_sphereCenter - colsestPoint);
	if (distance <= _sphereRadius) {
		return true;
	}

	return false;
}

bool CollisionCheck::SphereVsSphere(const Vector3& _sphere1Center, float _sphere1Radius, const Vector3& _sphere2Center, float _sphere2Radius) {
	float distance = Vector3::Length(_sphere1Center - _sphere2Center);
	if (distance <= _sphere1Radius + _sphere2Radius) {
		return true;
	}

	return false;
}
