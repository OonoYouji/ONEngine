#define NOMINMAX
#include "CollisionCheck.h"


bool CollisionCheck::LineVsSphere(const Vector3& _lineStart, const Vector3& _lineEnd, const Vector3& /*_sphereCenter*/, float /*_sphereRadius*/) {

	Vector3 lineDiff = _lineEnd - _lineStart;


	return false;
}

bool CollisionCheck::LineVsTriangle(const Vector3& _lineStart, const Vector3& _lineEnd, const std::array<Vector3, 3>& _triangleVertices) {

	Vector3&& lineDiff = _lineEnd - _lineStart;

	/// 三角形の頂点同士のベクトルを計算
	Vector3&& v01 = _triangleVertices[1] - _triangleVertices[0];
	Vector3&& v12 = _triangleVertices[2] - _triangleVertices[1];
	Vector3&& v20 = _triangleVertices[0] - _triangleVertices[2];

	/// 三角形の法線ベクトルと平面の距離を計算
	Vector3&& normal = Vector3::Cross(v01, v12);
	float distance = Vector3::Dot(
		(_triangleVertices[0] + _triangleVertices[1] + _triangleVertices[2]) / 3.0f, /// 3頂点の平均が中心
		normal
	);

	/// 線分と平面の交点を計算
	float dot = Vector3::Dot(normal, lineDiff);

	/// 平面と線分が平行の場合は衝突しない
	if (dot == 0.0f) {
		return false;
	}

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

bool CollisionCheck::CubeVsCube(
	const Vector3& _cube1Position, const Vector3& _cube1Size, const Vector3& _cube2Position, const Vector3& _cube2Size,
	Vector3* _outNormal, float* _outPenetration) {

	Vector3&& aMin = (_cube1Position - _cube1Size / 2.0f);
	Vector3&& aMax = (_cube1Position + _cube1Size / 2.0f);
	Vector3&& bMin = (_cube2Position - _cube2Size / 2.0f);
	Vector3&& bMax = (_cube2Position + _cube2Size / 2.0f);

	if (!(aMin.x <= bMax.x && aMax.x >= bMin.x)) { return false; }
	if (!(aMin.y <= bMax.y && aMax.y >= bMin.y)) { return false; }
	if (!(aMin.z <= bMax.z && aMax.z >= bMin.z)) { return false; }


	/// 各軸ごとのめりこみ量を計算
	float overlapX = std::min(aMax.x - bMin.x, bMax.x - aMin.x);
	float overlapY = std::min(aMax.y - bMin.y, bMax.y - aMin.y);
	float overlapZ = std::min(aMax.z - bMin.z, bMax.z - aMin.z);
	float penetration = overlapX;
	Vector3 normal;

	Vector3 delta = _cube2Position - _cube1Position;

	/// どの軸が最も浅いめりこみかを調べる かつ 法線を設定
	if (overlapY < penetration) {
		penetration = overlapY;
		normal = (delta.y > 0.0f) ? Vector3::kUp : Vector3::kDown;
	} else {
		normal = (delta.x > 0.0f) ? Vector3::kRight : Vector3::kLeft;
	}

	if (overlapZ < penetration) {
		penetration = overlapZ;
		normal = (delta.z > 0.0f) ? Vector3::kFront : Vector3::kBack;
	}

	if (_outNormal) {
		*_outNormal = normal;
	}
	if (_outPenetration) {
		*_outPenetration = penetration;
	}

	return true;
}

bool CollisionCheck::CubeVsSphere(
	const Vector3& _cubePosition, const Vector3& _cubeSize, const Vector3& _sphereCenter, float _sphereRadius,
	Vector3* _outClosestPoint, float* _outDistance) {

	Vector3&& cubeMin = _cubePosition - _cubeSize / 2.0f;
	Vector3&& cubeMax = _cubePosition + _cubeSize / 2.0f;

	Vector3&& closestPoint = {
		std::clamp(_sphereCenter.x, cubeMin.x, cubeMax.x),
		std::clamp(_sphereCenter.y, cubeMin.y, cubeMax.y),
		std::clamp(_sphereCenter.z, cubeMin.z, cubeMax.z)
	};

	float distance = Vector3::Length(_sphereCenter - closestPoint);

	/// 出力パラメータの設定
	if (_outClosestPoint) {
		*_outClosestPoint = closestPoint;
	}
	if (_outDistance) {
		*_outDistance = distance;
	}


	if (distance <= _sphereRadius) {
		return true;
	}

	return false;
}

bool CollisionCheck::CubeVsCapsule(const Vector3& _cubePosition, const Vector3& _cubeSize, const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius) {
	Vector3 capsulePoint, boxPoint;
	CollisionMath::ClosestPointsSegmentAABB(
		_capsuleStart, _capsuleEnd,
		_cubePosition - _cubeSize / 2.0f, _cubePosition + _cubeSize / 2.0f,
		capsulePoint, boxPoint
	);

	float distance = Vector3::Length(capsulePoint - boxPoint);

	return distance < _capsuleRadius;
}

bool CollisionCheck::SphereVsSphere(const Vector3& _sphere1Center, float _sphere1Radius, const Vector3& _sphere2Center, float _sphere2Radius) {
	float distance = Vector3::Length(_sphere1Center - _sphere2Center);
	if (distance <= _sphere1Radius + _sphere2Radius) {
		return true;
	}

	return false;
}

bool CollisionCheck::SphereVsCapsule(const Vector3& _sphereCenter, float _sphereRadius, const Vector3& _capsuleStart, const Vector3& _capsuleEnd, float _capsuleRadius) {
	/// 最近接点を求める
	Vector3 capsuleDirection = _capsuleEnd - _capsuleStart;
	float capsuleLength = Vector3::Length(capsuleDirection);

	if (capsuleLength == 0.0f) {
		/// カプセルの長さが0の場合、カプセルは点として扱う
		return SphereVsSphere(
			_sphereCenter, _sphereRadius,
			_capsuleStart, _capsuleRadius
		);
	}

	Vector3 dir = capsuleDirection * (1.0f / capsuleLength);
	float t = Vector3::Dot(_sphereCenter - _capsuleStart, dir);
	if (t < 0.0f) {
		/// 球の中心がカプセルの始点より前にある場合
		t = 0.0f;
	} else if (t > capsuleLength) {
		/// 球の中心がカプセルの終点より後ろにある場合
		t = capsuleLength;
	}

	Vector3 closestPoint = _capsuleStart + dir * t;
	float distance = Vector3::Length(_sphereCenter - closestPoint);
	return distance < (_sphereRadius + _capsuleRadius);
}

Vector3 CollisionMath::ClosestPointOnAABB(const Vector3& _point, const Vector3& _aabbMin, const Vector3& _aabbMax) {
	/// 各軸ごとにクランプして最も近い点を求める
	return {
		std::max(_aabbMin.x, std::min(_point.x, _aabbMax.x)),
		std::max(_aabbMin.y, std::min(_point.y, _aabbMax.y)),
		std::max(_aabbMin.z, std::min(_point.z, _aabbMax.z))
	};
}

void CollisionMath::ClosestPointsSegmentAABB(const Vector3& _lineStart, const Vector3& _lineEnd, const Vector3& _aabbMin, const Vector3& _aabbMax, Vector3& _outSegmentPoint, Vector3& _outAABBPoint) {
	Vector3 segmentDirection = _lineEnd - _lineStart;
	float segmentLength = Vector3::Length(segmentDirection);

	/// 線分の長さが0の場合、始点を返す
	if (segmentLength == 0.0f) {
		_outSegmentPoint = _lineStart;
		_outAABBPoint = ClosestPointOnAABB(_lineStart, _aabbMin, _aabbMax);
		return;
	}

	/// Segmentの方向を正規化
	Vector3 dir = segmentDirection * (1.0f / segmentLength);

	float t = 0.0f;
	Vector3 closest = _lineStart;

	/// 各軸ごとにAABBの範囲外にある場合、tを更新
	for (int i = 0; i < 3; ++i) {

		/// 各軸の成分を取得
		float segStart = (&_lineStart.x)[i];
		float segEnd = (&_lineEnd.x)[i];
		float aabbMin = (&_aabbMin.x)[i];
		float aabbMax = (&_aabbMax.x)[i];

		/// 線分の成分の差を計算
		float segDelta = segEnd - segStart;

		if (segStart < aabbMin && segDelta > 0.0f) {
			t = std::max(t, (aabbMin - segStart) / segDelta);
		} else if (segStart > aabbMax && segDelta < 0.0f) {
			t = std::max(t, (aabbMax - segStart) / segDelta);
		}
	}


	/// 0.0 - 1.0の範囲にクランプ
	t = std::clamp(t, 0.0f, 1.0f);

	/// 線分上の最近接点を計算
	_outSegmentPoint = _lineStart + segmentDirection * t;

	/// AABB上の最近接点を計算
	_outAABBPoint = ClosestPointOnAABB(_outSegmentPoint, _aabbMin, _aabbMax);

}
