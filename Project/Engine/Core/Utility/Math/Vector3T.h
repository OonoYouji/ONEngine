#pragma once

#ifdef max
#undef max
#endif


/// std
#include <cmath>
#include <limits>
#include <format>
#include <type_traits>
#include <algorithm>

/// externals
#include <nlohmann/json.hpp>


namespace ONEngine {

/// ///////////////////////////////////////////////////
/// Vector3のテンプレート版
/// ///////////////////////////////////////////////////
template <typename T>
struct Vector3T final {
	/// ===================================================
	/// public : objects
	/// ===================================================

	T x, y, z;


	/// ===================================================
	/// public : constants
	/// ===================================================

	static const Vector3T<T> Zero;
	static const Vector3T<T> One;

	static const Vector3T<T> Left;
	static const Vector3T<T> Right;
	static const Vector3T<T> Up;
	static const Vector3T<T> Down;
	static const Vector3T<T> Forward;
	static const Vector3T<T> Back;

	static const Vector3T<T> Infinity;
	static const Vector3T<T> NegativeInfinity;

	static const Vector3T<T> Max;
	static const Vector3T<T> Min;


	/// ===================================================
	/// public : constructors
	/// ===================================================

	Vector3T() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) {}
	Vector3T(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	Vector3T(const Vector3T&) = default;
	Vector3T(Vector3T&&) = default;

	Vector3T& operator=(const Vector3T&) = default;
	Vector3T& operator=(Vector3T&&) = default;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	/// @brief ベクトルの長さを取得
	/// @param _v ベクトル
	/// @return ベクトルの長さ
	static T Length(const Vector3T<T>& _v) {
		return static_cast<T>(std::sqrt(_v.x * _v.x + _v.y * _v.y + _v.z * _v.z));
	}

	/// @brief ベクトルの長さの二乗を取得
	/// @param _v ベクトル
	/// @return ベクトルの長さの二乗
	static T LengthSquared(const Vector3T<T>& _v) {
		return _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
	}

	/// @brief ベクトルの正規化
	/// @param _v ベクトル
	/// @return 正規化されたベクトル
	static Vector3T<T> Normalize(const Vector3T<T>& _v) {
		T length = Length(_v);
		if (length == static_cast<T>(0)) {
			return Vector3T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		}
		return Vector3T<T>(_v.x / length, _v.y / length, _v.z / length);
	}

	/// @brief ベクトルの内積を取得
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @return ベクトルの内積
	static T Dot(const Vector3T<T>& _a, const Vector3T<T>& _b) {
		return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
	}

	/// @brief ベクトルの外積
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @return ベクトルの外積
	static Vector3T<T> Cross(const Vector3T<T>& _a, const Vector3T<T>& _b) {
		return Vector3T<T>(
			_a.y * _b.z - _a.z * _b.y,
			_a.z * _b.x - _a.x * _b.z,
			_a.x * _b.y - _a.y * _b.x
		);
	}

	/// @brief ベクトルの線形補完
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @param _t 補間係数
	/// @return 補間されたベクトル
	static Vector3T<T> Lerp(const Vector3T<T>& _a, const Vector3T<T>& _b, T _t) {
		return Vector3T<T>(
			_a.x + (_b.x - _a.x) * _t,
			_a.y + (_b.y - _a.y) * _t,
			_a.z + (_b.z - _a.z) * _t
		);
	}

	/// @brief ベクトルの球面線形補完
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @param _t 補間係数
	/// @return 補間されたベクトル
	static Vector3T<T> Slerp(const Vector3T<T>& _a, const Vector3T<T>& _b, T _t) {
		T dot = Dot(Normalize(_a), Normalize(_b));
		dot = std::clamp(dot, static_cast<T>(-1), static_cast<T>(1));
		T theta = std::acos(dot) * _t;
		Vector3T<T> relativeVec = _b - _a * dot;
		relativeVec = Normalize(relativeVec);
		return (_a * std::cos(theta)) + (relativeVec * std::sin(theta));
	}


	/// ===================================================
	/// public : methods
	/// ===================================================

	/// @brief ベクトルの長さを取得
	/// @return ベクトルの長さ
	T Length() const {
		return Length(*this);
	}

	/// @brief ベクトルの長さの二乗を取得
	/// @return ベクトルの長さの二乗
	T LengthSquared() const {
		return LengthSquared(*this);
	}

	/// @brief ベクトルの正規化
	/// @return 正規化されたベクトル
	Vector3T<T> Normalize() const {
		return Normalize(*this);
	}

	/// @brief ベクトルの内積を取得
	/// @param _other もう一つのベクトル
	/// @return ベクトルの内積
	T Dot(const Vector3T<T>& _other) const {
		return Dot(*this, _other);
	}

	/// @brief ベクトルの外積を取得
	/// @param _other もう一つのベクトル
	/// @return ベクトルの外積
	Vector3T<T> Cross(const Vector3T<T>& _other) const {
		return Cross(*this, _other);
	}


	/// ===================================================
	/// public : operators
	/// ===================================================

	Vector3T<T>& operator+=(const Vector3T<T>& _other) {
		x += _other.x;
		y += _other.y;
		z += _other.z;
		return *this;
	}

	Vector3T<T>& operator-=(const Vector3T<T>& _other) {
		x -= _other.x;
		y -= _other.y;
		z -= _other.z;
		return *this;
	}

	Vector3T<T>& operator*=(const Vector3T<T>& _other) {
		x *= _other.x;
		y *= _other.y;
		z *= _other.z;
		return *this;
	}

	Vector3T<T>& operator*=(T _scalar) {
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		return *this;
	}

	Vector3T<T>& operator/=(const Vector3T<T>& _other) {
		x /= _other.x;
		y /= _other.y;
		z /= _other.z;
		return *this;
	}

	Vector3T<T>& operator/=(T _scalar) {
		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		return *this;
	}


};


// クラス外で定義
template <typename T>
const Vector3T<T> Vector3T<T>::Zero = Vector3T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

template <typename T>
const Vector3T<T> Vector3T<T>::One = Vector3T<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));

template <typename T>
const Vector3T<T> Vector3T<T>::Left = Vector3T<T>(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0));

template <typename T>
const Vector3T<T> Vector3T<T>::Right = Vector3T<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));

template <typename T>
const Vector3T<T> Vector3T<T>::Up = Vector3T<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));

template <typename T>
const Vector3T<T> Vector3T<T>::Down = Vector3T<T>(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0));

template <typename T>
const Vector3T<T> Vector3T<T>::Forward = Vector3T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

template <typename T>
const Vector3T<T> Vector3T<T>::Back = Vector3T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1));

template <typename T>
const Vector3T<T> Vector3T<T>::Infinity = Vector3T<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());

template <typename T>
const Vector3T<T> Vector3T<T>::NegativeInfinity = Vector3T<T>(-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity());

template <typename T>
const Vector3T<T> Vector3T<T>::Max = Vector3T<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max());

template <typename T>
const Vector3T<T> Vector3T<T>::Min = Vector3T<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest());


/// ///////////////////////////////////////////////////
/// operator
/// ///////////////////////////////////////////////////

template <typename T>
inline Vector3T<T> operator+(const Vector3T<T>& _a, const Vector3T<T>& _b) {
	return Vector3T<T>(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z);
}

template <typename T>
inline Vector3T<T> operator-(const Vector3T<T>& _a, const Vector3T<T>& _b) {
	return Vector3T<T>(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z);
}

template <typename T>
inline Vector3T<T> operator*(const Vector3T<T>& _a, const Vector3T<T>& _b) {
	return Vector3T<T>(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z);
}

template <typename T>
inline Vector3T<T> operator*(const Vector3T<T>& _v, T _scalar) {
	return Vector3T<T>(_v.x * _scalar, _v.y * _scalar, _v.z * _scalar);
}

template <typename T>
inline Vector3T<T> operator*(T _scalar, const Vector3T<T>& _v) {
	return Vector3T<T>(_v.x * _scalar, _v.y * _scalar, _v.z * _scalar);
}

template <typename T>
inline Vector3T<T> operator/(const Vector3T<T>& _a, const Vector3T<T>& _b) {
	return Vector3T<T>(_a.x / _b.x, _a.y / _b.y, _a.z / _b.z);
}

template <typename T>
inline Vector3T<T> operator/(const Vector3T<T>& _v, T _scalar) {
	return Vector3T<T>(_v.x / _scalar, _v.y / _scalar, _v.z / _scalar);
}

template <typename T>
inline Vector3T<T> operator-(const Vector3T<T>& _v) {
	return Vector3T<T>(-_v.x, -_v.y, -_v.z);
}

template <typename T>
inline Vector3T<T> operator+(const Vector3T<T>& _v) {
	return _v;
}

template <typename T>
bool operator==(const Vector3T<T>& _a, const Vector3T<T>& _b) {
	return _a.x == _b.x && _a.y == _b.y && _a.z == _b.z;
}

template <typename T>
bool operator!=(const Vector3T<T>& _a, const Vector3T<T>& _b) {
	return !(_a == _b);
}



template <typename T>
void from_json(const nlohmann::json& _j, Vector3T<T>& _v) {
	_v.x = _j.at("x").get<T>();
	_v.y = _j.at("y").get<T>();
	_v.z = _j.at("z").get<T>();
}

template <typename T>
void to_json(nlohmann::json& _j, const Vector3T<T>& _v) {
	_j = nlohmann::json{
		{ "x", _v.x },
		{ "y", _v.y },
		{ "z", _v.z }
	};
}

}