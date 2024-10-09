#pragma once

#include "Math/Vector3.h"

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
	/// public : static methods
	/// ===================================================

	static float Lenght(const Quaternion& q);
	static Quaternion Normalize(const Quaternion& q);


	/// <summary>
	/// ある軸を基にquaternionを計算する
	/// </summary>
	/// <param name="axis">   : 回転の軸となるベクトル						 </param>
	/// <param name="theta">  : 回転率									 </param>
	/// <returns>             return: axisを基にtheta分回転させたquaternion </returns>
	static Quaternion MakeFromAxis(const Vec3& axis, float theta);

	/// <summary>
	/// 特定の方向を見る関数
	/// </summary>
	/// <param name="position"></param>
	/// <param name="target"></param>
	/// <param name="up"></param>
	/// <returns></returns>
	static Quaternion LockAt(const Vec3& position, const Vec3& target, const Vec3& up);




	/// ===================================================
	/// public : objects
	/// ===================================================

	float x, y, z, w;


	/// ===================================================
	/// public : operator
	/// ===================================================

	inline Quaternion& operator*= (const Quaternion& other);

};



/// ===================================================
/// quaternion operator
/// ===================================================

inline Quaternion operator* (const Quaternion& q1, const Quaternion& q2) {
	return {
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
	};
}

inline Quaternion operator/ (const Quaternion& q, float value) {
	Quaternion result;
	result.w = q.w / value;
	result.x = q.x / value;
	result.y = q.y / value;
	result.z = q.z / value;
	return result;
}

inline Quaternion operator/ (float value, const Quaternion& q) {
	Quaternion result;
	result.w = q.w / value;
	result.x = q.x / value;
	result.y = q.y / value;
	result.z = q.z / value;
	return result;
}


inline Quaternion& Quaternion::operator*=(const Quaternion& other) {
	*this = *this * other;
	return *this;
}