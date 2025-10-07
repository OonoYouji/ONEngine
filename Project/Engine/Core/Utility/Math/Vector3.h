#pragma once

/// std
#include <vector>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Vector2.h"

/// ===================================================
/// 3次元ベクトル
/// ===================================================
class Vector3 final {
public:

	Vector3();
	Vector3(float _x, float _y, float _z);
	Vector3(const Vector2& _v, float _z);
	Vector3(const Vector3&) = default;
	Vector3(Vector3&&) = default;

public:

	float x, y, z;

public:

	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Vector3 kUp;		// ( 0,  1,  0 )
	static const Vector3 kDown;		// ( 0, -1,  0 )
	static const Vector3 kLeft;		// (-1,  0,  0 )
	static const Vector3 kRight;	// ( 1,  0,  0 )
	static const Vector3 kFront;	// ( 0,  0,  1 )
	static const Vector3 kBack;		// ( 0,  0, -1 )

	static const Vector3 kOne;		// ( 1,  1,  1 )
	static const Vector3 kZero;		// ( 0,  0,  0 )

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	float Len() const;
	float LengthSquared() const;

	Vector3 Normalize() const;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	static float Length(const Vector3& _v);
	static float LengthSquared(const Vector3& _v);

	static Vector3 Normalize(const Vector3& _v);

	static Vector3 Lerp(const Vector3& _start, const Vector3& _end, float _t);
	static Vector3 Slerp(const Vector3& _start, const Vector3& _end, float _t);

	static Vector3 Cross(const Vector3& _v1, const Vector3& _v2);
	static float Dot(const Vector3& _v1, const Vector3& _v2);

	static Vector3 MaxDotVector(const Vector3& _direction, const std::vector<Vector3>& _vertices);
	static Vector3 MinDotVector(const Vector3& _direction, const std::vector<Vector3>& _vertices);

public:

	/// ===================================================
	/// public : operator overload
	/// ===================================================

	inline Vector3& operator= (const Vector3& _other);
	inline Vector3& operator+= (const Vector3& _other);
	inline Vector3& operator-= (const Vector3& _other);
	inline Vector3& operator/= (const Vector3& _other);
	inline Vector3& operator*= (const Vector3& _other);

	inline Vector3& operator/= (float _scaler);
	inline Vector3& operator*= (float _scaler);

	inline Vector3 operator- () const;
	inline Vector3 operator+ () const;

};

/// ===================================================
/// 演算子オーバーロード
/// ===================================================
#pragma region operator overload

inline Vector3 operator+ (const Vector3& _v1, const Vector3& _v2) {
	return {
		_v1.x + _v2.x,
		_v1.y + _v2.y,
		_v1.z + _v2.z
	};
}


inline Vector3 operator- (const Vector3& _v1, const Vector3& _v2) {
	return {
		_v1.x - _v2.x,
		_v1.y - _v2.y,
		_v1.z - _v2.z
	};
}


inline Vector3 operator/ (const Vector3& _v1, const Vector3& _v2) {
	return {
		_v1.x / _v2.x,
		_v1.y / _v2.y,
		_v1.z / _v2.z
	};
}


inline Vector3 operator/ (const Vector3& _v, float _scaler) {
	return {
		_v.x / _scaler,
		_v.y / _scaler,
		_v.z / _scaler
	};
}


inline Vector3 operator* (const Vector3& _v1, const Vector3& _v2) {
	return {
		_v1.x * _v2.x,
		_v1.y * _v2.y,
		_v1.z * _v2.z
	};
}


inline Vector3 operator* (const Vector3& _v, float _scaler) {
	return {
		_v.x * _scaler,
		_v.y * _scaler,
		_v.z * _scaler
	};
}


inline Vector3 operator* (float _scaler, const Vector3& _v) {
	return _v * _scaler;
}


inline bool operator!=(const Vector3& _v1, const Vector3& _v2) {
	return _v1.x != _v2.x || _v1.y != _v2.y || _v1.z != _v2.z;
}

inline bool operator==(const Vector3& _v1, const Vector3& _v2) {
	return _v1.x == _v2.x && _v1.y == _v2.y && _v1.z == _v2.z;
}


/// ===================================================
/// Vector3 : operator overload
/// ===================================================

inline Vector3& Vector3::operator= (const Vector3& _other) {
	this->x = _other.x;
	this->y = _other.y;
	this->z = _other.z;
	return *this;
}


inline Vector3& Vector3::operator+= (const Vector3& _other) {
	(*this) = *this + _other;
	return *this;
}


inline Vector3& Vector3::operator-= (const Vector3& _other) {
	(*this) = *this - _other;
	return *this;
}


inline Vector3& Vector3::operator/= (const Vector3& _other) {
	(*this) = *this / _other;
	return *this;
}


inline Vector3& Vector3::operator*= (const Vector3& _other) {
	(*this) = *this * _other;
	return *this;
}


inline Vector3& Vector3::operator/= (float _scaler) {
	(*this) = *this / _scaler;
	return *this;
}


inline Vector3& Vector3::operator*= (float _scaler) {
	(*this) = *this * _scaler;
	return *this;
}


inline Vector3 Vector3::operator-() const {
	return (*this) * -1.0f;
}


inline Vector3 Vector3::operator+() const {
	return (*this);
}

#pragma endregion


/// json 変換
void from_json(const nlohmann::json& _j, Vector3& _v);
void to_json(nlohmann::json& _j, const Vector3& _v);