#include "Quaternion.h"

#include <cmath>


inline Quaternion::Quaternion() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

float Quaternion::Lenght(const Quaternion& q) {
	return std::sqrt(
		q.x * q.x +
		q.y * q.y +
		q.z * q.z +
		q.w * q.w
	);
}

Quaternion Quaternion::Normalize(const Quaternion& q) {
	float len = Lenght(q);
	if(len != 0.0f) {
		return q / len;
	}
	return q;
}

Quaternion Quaternion::MakeFromAxis(const Vec3& axis, float theta) {
	float halfAngle = theta * 0.5f;
	float sinHalfAngle = std::sin(halfAngle);

	Vector3 normalizedAxis = axis.Normalize();

	float w = std::cos(halfAngle);
	float x = normalizedAxis.x * sinHalfAngle;
	float y = normalizedAxis.y * sinHalfAngle;
	float z = normalizedAxis.z * sinHalfAngle;

	return Quaternion(x, y, z, w);
}



Quaternion Quaternion::Inverse() const {
	Quaternion conjugate = Conjugate(); // 共役を計算
	float norm = Norm();                // ノルムを計算
	if(norm == 0.0f) {
		// ノルムがゼロの場合、逆クォータニオンは定義されないため、適切なエラー処理を追加
	}

	float normSquared = norm * norm;    // ノルムの二乗
	return conjugate / normSquared;
	//return { conjugate.w / normSquared, conjugate.x / normSquared, conjugate.y / normSquared, conjugate.z / normSquared };
}