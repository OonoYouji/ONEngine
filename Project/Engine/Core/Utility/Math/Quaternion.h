#pragma once

/// std
#include <cmath>

/// engine
#include "Vector3.h"
#include "Matrix4x4.h"

/// ===================================================
/// 三次元での回転を表すクラス
/// ===================================================
class Quaternion final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Quaternion();
	Quaternion(float _x, float _y, float _z, float _w);



	/// ===================================================
	/// public : objects
	/// ===================================================

	float x, y, z, w;



	/// ===================================================
	/// public : static methods
	/// ===================================================

	static float Length(const Quaternion& _q);
	static Quaternion Normalize(const Quaternion& _q);
	static Vector3 Transform(const Vector3& _v, const Quaternion& _q);

	static Quaternion Lerp(const Quaternion& _start, const Quaternion& _end, float _t);

	static Quaternion MakeFromAxis(const Vec3& _axis, float _theta);

	static Matrix4x4 MakeRotateAxisAngle(const Vec3& axis, float theta);

	static Quaternion LockAt(const Vec3& _position, const Vec3& _target, const Vec3& _up);
	static Quaternion LockAt(const Vec3& _position, const Vec3& _target);

	static Quaternion Slerp(const Quaternion& _start, const Quaternion& _end, float _t);

	/// ===================================================
	/// public : methods
	/// ===================================================

	// クォータニオンの共役を計算する関数
	Quaternion Conjugate() const {
		return { -x, -y, -z, w };
	}

	// クォータニオンのノルムを計算する関数
	float Length() const {
		return std::sqrt(w * w + x * x + y * y + z * z);
	}

	// 逆クォータニオンを計算する関数
	Quaternion Inverse() const;



	/// ===================================================
	/// public : operator
	/// ===================================================

	inline Quaternion& operator*= (const Quaternion& _other);



};



/// ===================================================
/// quaternion operator
/// ===================================================

inline Quaternion operator* (const Quaternion& _q1, const Quaternion& _q2) {
	return {
		_q1.w * _q2.x + _q1.x * _q2.w + _q1.y * _q2.z - _q1.z * _q2.y,
		_q1.w * _q2.y + _q1.y * _q2.w + _q1.z * _q2.x - _q1.x * _q2.z,
		_q1.w * _q2.z + _q1.z * _q2.w + _q1.x * _q2.y - _q1.y * _q2.x,
		_q1.w * _q2.w - _q1.x * _q2.x - _q1.y * _q2.y - _q1.z * _q2.z
	};
}

inline Quaternion operator/ (const Quaternion& _q, float _value) {
	Quaternion result;
	result.w = _q.w / _value;
	result.x = _q.x / _value;
	result.y = _q.y / _value;
	result.z = _q.z / _value;
	return result;
}


inline Quaternion& Quaternion::operator*=(const Quaternion& _other) {
	*this = *this * _other;
	return *this;
}
