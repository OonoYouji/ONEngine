#pragma once
#include<cmath>
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


	/// ===================================================
	/// public : objects
	/// ===================================================

	float x, y, z, w;


	/// ===================================================
	/// public : operator
	/// ===================================================

	inline Quaternion& operator*= (const Quaternion& other);

	// クォータニオンの共役を計算する関数
	Quaternion Conjugate() const {
		return { w, -x, -y, -z };
	}

	// クォータニオンのノルムを計算する関数
	float Norm() const {
		return std::sqrt(w * w + x * x + y * y + z * z);
	}

	// 逆クォータニオンを計算する関数
	Quaternion Inverse() const {
		Quaternion conjugate = Conjugate(); // 共役を計算
		float norm = Norm();                // ノルムを計算
		if (norm == 0.0f) {
			// ノルムがゼロの場合、逆クォータニオンは定義されないため、適切なエラー処理を追加
		}

		float normSquared = norm * norm;    // ノルムの二乗
		return { conjugate.w / normSquared, conjugate.x / normSquared, conjugate.y / normSquared, conjugate.z / normSquared };
	}


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
