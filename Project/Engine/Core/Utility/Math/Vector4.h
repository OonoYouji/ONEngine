#pragma once

#include "Vector3.h"

/// ===================================================
/// 4次元ベクトル
/// ===================================================
class Vector4 final {
public:

	Vector4();
	Vector4(float _x, float _y, float _z, float _w);
	Vector4(const Vector3& _v, float _w);
	Vector4(const Vector4&) = default;
	Vector4(Vector4&&)      = default;

public:

	float x, y, z, w;



public:

	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Vector4 k1000;
	static const Vector4 k0100;
	static const Vector4 k0010;
	static const Vector4 k0001;

	static const Vector4 kWhite;
	static const Vector4 kRed;
	static const Vector4 kGreen;
	static const Vector4 kBlue;

	static const Vector4 kZero;
	
	
	/// @brief Vector4の線形補間
	/// @param _v1 開始値
	/// @param _v2 終了値
	/// @param _t 媒介変数
	/// @return _v1と_v2の間を_tで補間した値
	static Vector4 Lerp(const Vector4& _v1, const Vector4& _v2, float _t);

	/// @brief Vector4の長さを取得
	/// @param _v 計算するVector4
	/// @return 長さ
	static float Length(const Vector4& _v);

	/// @brief 
	/// @param _v 
	/// @return 
	static Vector4 Normalize(const Vector4& _v);

	static Vector4 Convert(const Vector3& _v, float _w = 1.0f);

	static Vector3 Convert(const Vector4& _v);

public:

	/// ===================================================
	/// public : operator overload
	/// ===================================================


	inline Vector4& operator= (const Vector4& _other);
	inline Vector4& operator+= (const Vector4& _other);
	inline Vector4& operator-= (const Vector4& _other);
	inline Vector4& operator/= (const Vector4& _other);
	inline Vector4& operator*= (const Vector4& _other);

	inline Vector4& operator/= (float _scaler);
	inline Vector4& operator*= (float _scaler);

	inline Vector4 operator- ();
	inline Vector4 operator+ ();


};



/// ===================================================
/// 演算子オーバーロード
/// ===================================================
#pragma region operator overload

inline Vector4 operator+ (const Vector4& _v1, const Vector4& _v2) {
	return {
		_v1.x + _v2.x,
		_v1.y + _v2.y,
		_v1.z + _v2.z,
		_v1.w + _v2.w
	};
}


inline Vector4 operator- (const Vector4& _v1, const Vector4& _v2) {
	return {
		_v1.x - _v2.x,
		_v1.y - _v2.y,
		_v1.z - _v2.z,
		_v1.w - _v2.w
	};
}


inline Vector4 operator/ (const Vector4& _v1, const Vector4& _v2) {
	return {
		_v1.x / _v2.x,
		_v1.y / _v2.y,
		_v1.z / _v2.z,
		_v1.w / _v2.w
	};
}

inline Vector4 operator/ (const Vector4& _v, float _scaler) {
	return {
		_v.x / _scaler,
		_v.y / _scaler,
		_v.z / _scaler,
		_v.w / _scaler
	};
}


inline Vector4 operator* (const Vector4& _v1, const Vector4& _v2) {
	return {
		_v1.x * _v2.x,
		_v1.y * _v2.y,
		_v1.z * _v2.z,
		_v1.w * _v2.w
	};
}


inline Vector4 operator* (const Vector4& _v, float _scaler) {
	return {
		_v.x * _scaler,
		_v.y * _scaler,
		_v.z * _scaler,
		_v.w * _scaler
	};
}


inline Vector4 operator* (float _scaler, const Vector4& _v) {
	return _v * _scaler;
}


inline bool operator!=(const Vector4& _v1, const Vector4& _v2) {
	return _v1.x != _v2.x || _v1.y != _v2.y || _v1.z != _v2.z || _v1.w != _v2.w;
}


inline bool operator==(const Vector4& _v1, const Vector4& _v2) {
	return _v1.x == _v2.x && _v1.y == _v2.y && _v1.z == _v2.z && _v1.w == _v2.w;
}


/// ===================================================
/// Vector4 : operator overload
/// ===================================================


inline Vector4& Vector4::operator=(const Vector4& _other) {
	this->x = _other.x;
	this->y = _other.y;
	this->z = _other.z;
	this->w = _other.w;

	return *this;
}

inline Vector4& Vector4::operator+= (const Vector4& _other) {
	(*this) = *this + _other;
	return *this;
}


inline Vector4& Vector4::operator-= (const Vector4& _other) {
	(*this) = *this - _other;
	return *this;
}


inline Vector4& Vector4::operator/= (const Vector4& _other) {
	(*this) = *this / _other;
	return *this;
}


inline Vector4& Vector4::operator*= (const Vector4& _other) {
	(*this) = *this * _other;
	return *this;
}


inline Vector4& Vector4::operator/= (float _scaler) {
	(*this) = *this / _scaler;
	return *this;
}


inline Vector4& Vector4::operator*= (float _scaler) {
	(*this) = *this * _scaler;
	return *this;
}


inline Vector4 Vector4::operator-() {
	return (*this) * -1.0f;
}


inline Vector4 Vector4::operator+() {
	return (*this);
}

#pragma endregion


using Vec4 = Vector4;