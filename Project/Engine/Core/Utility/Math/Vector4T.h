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
/// Vector4のテンプレート版
/// ///////////////////////////////////////////////////
template <typename T>
struct Vector4T final {
	/// ===================================================
	/// public : objects
	/// ===================================================

	T x, y, z, w;


	/// ===================================================
	/// public : constants
	/// ===================================================

	static const Vector4T<T> Zero;
	static const Vector4T<T> One;

	static const Vector4T<T> Red;
	static const Vector4T<T> Green;
	static const Vector4T<T> Blue;
	static const Vector4T<T> White;

	static const Vector4T<T> Infinity;
	static const Vector4T<T> NegativeInfinity;

	static const Vector4T<T> Max;
	static const Vector4T<T> Min;


	/// ===================================================
	/// public : constructors
	/// ===================================================

	Vector4T() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)), w(static_cast<T>(0)) {}
	Vector4T(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4T(const Vector4T&) = default;
	Vector4T(Vector4T&&) = default;

	Vector4T& operator=(const Vector4T&) = default;
	Vector4T& operator=(Vector4T&&) = default;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	/// @brief ベクトルの長さを取得
	/// @param _v ベクトル
	/// @return ベクトルの長さ
	static T Length(const Vector4T<T>& _v) {
		return static_cast<T>(std::sqrt(_v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w));
	}

	/// @brief ベクトルの長さの二乗を取得
	/// @param _v ベクトル
	/// @return ベクトルの長さの二乗
	static T LengthSquared(const Vector4T<T>& _v) {
		return _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
	}

	/// @brief ベクトルの正規化
	/// @param _v ベクトル
	/// @return 正規化されたベクトル
	static Vector4T<T> Normalize(const Vector4T<T>& _v) {
		T length = Length(_v);
		if (length == static_cast<T>(0)) {
			return Vector4T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		}
		return Vector4T<T>(_v.x / length, _v.y / length, _v.z / length, _v.w / length);
	}

	/// @brief ベクトルの内積を取得
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @return ベクトルの内積
	static T Dot(const Vector4T<T>& _a, const Vector4T<T>& _b) {
		return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
	}

	/// @brief ベクトルの線形補完
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @param _t 補間係数
	/// @return 補間されたベクトル
	static Vector4T<T> Lerp(const Vector4T<T>& _a, const Vector4T<T>& _b, T _t) {
		return Vector4T<T>(
			_a.x + (_b.x - _a.x) * _t,
			_a.y + (_b.y - _a.y) * _t,
			_a.z + (_b.z - _a.z) * _t,
			_a.w + (_b.w - _a.w) * _t
		);
	}

	/// @brief ベクトルの球面線形補完
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @param _t 補間係数
	/// @return 補間されたベクトル
	static Vector4T<T> Slerp(const Vector4T<T>& _a, const Vector4T<T>& _b, T _t) {
		T dot = Dot(Normalize(_a), Normalize(_b));
		dot = std::clamp(dot, static_cast<T>(-1), static_cast<T>(1));
		T theta = std::acos(dot) * _t;
		Vector4T<T> relativeVec = _b - _a * dot;
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
	Vector4T<T> Normalize() const {
		return Normalize(*this);
	}

	/// @brief ベクトルの内積を取得
	/// @param _other もう一つのベクトル
	/// @return ベクトルの内積
	T Dot(const Vector4T<T>& _other) const {
		return Dot(*this, _other);
	}


	/// ===================================================
	/// public : operators
	/// ===================================================

	Vector4T<T>& operator+=(const Vector4T<T>& _other) {
		x += _other.x;
		y += _other.y;
		z += _other.z;
		w += _other.w;
		return *this;
	}

	Vector4T<T>& operator-=(const Vector4T<T>& _other) {
		x -= _other.x;
		y -= _other.y;
		z -= _other.z;
		w -= _other.w;
		return *this;
	}

	Vector4T<T>& operator*=(const Vector4T<T>& _other) {
		x *= _other.x;
		y *= _other.y;
		z *= _other.z;
		w *= _other.w;
		return *this;
	}

	Vector4T<T>& operator*=(T _scalar) {
		x *= _scalar;
		y *= _scalar;
		z *= _scalar;
		w *= _scalar;
		return *this;
	}

	Vector4T<T>& operator/=(const Vector4T<T>& _other) {
		x /= _other.x;
		y /= _other.y;
		z /= _other.z;
		w /= _other.w;
		return *this;
	}

	Vector4T<T>& operator/=(T _scalar) {
		x /= _scalar;
		y /= _scalar;
		z /= _scalar;
		w /= _scalar;
		return *this;
	}


};


// クラス外で定義
template <typename T>
const Vector4T<T> Vector4T<T>::Zero = Vector4T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

template <typename T>
const Vector4T<T> Vector4T<T>::One = Vector4T<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));

template <typename T>
const Vector4T<T> Vector4T<T>::White = Vector4T<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));

template <typename T>
const Vector4T<T> Vector4T<T>::Red = Vector4T<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

template <typename T>
const Vector4T<T> Vector4T<T>::Green = Vector4T<T>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(1));

template <typename T>
const Vector4T<T> Vector4T<T>::Blue = Vector4T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(1));

template <typename T>
const Vector4T<T> Vector4T<T>::Infinity = Vector4T<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());

template <typename T>
const Vector4T<T> Vector4T<T>::NegativeInfinity = Vector4T<T>(-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity());

template <typename T>
const Vector4T<T> Vector4T<T>::Max = Vector4T<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max());

template <typename T>
const Vector4T<T> Vector4T<T>::Min = Vector4T<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest());


/// ///////////////////////////////////////////////////
/// operator
/// ///////////////////////////////////////////////////

template <typename T>
inline Vector4T<T> operator+(const Vector4T<T>& _a, const Vector4T<T>& _b) {
	return Vector4T<T>(_a.x + _b.x, _a.y + _b.y, _a.z + _b.z, _a.w + _b.w);
}

template <typename T>
inline Vector4T<T> operator-(const Vector4T<T>& _a, const Vector4T<T>& _b) {
	return Vector4T<T>(_a.x - _b.x, _a.y - _b.y, _a.z - _b.z, _a.w - _b.w);
}

template <typename T>
inline Vector4T<T> operator*(const Vector4T<T>& _a, const Vector4T<T>& _b) {
	return Vector4T<T>(_a.x * _b.x, _a.y * _b.y, _a.z * _b.z, _a.w * _b.w);
}

template <typename T>
inline Vector4T<T> operator*(const Vector4T<T>& _v, T _scalar) {
	return Vector4T<T>(_v.x * _scalar, _v.y * _scalar, _v.z * _scalar, _v.w * _scalar);
}

template <typename T>
inline Vector4T<T> operator*(T _scalar, const Vector4T<T>& _v) {
	return Vector4T<T>(_v.x * _scalar, _v.y * _scalar, _v.z * _scalar, _v.w * _scalar);
}

template <typename T>
inline Vector4T<T> operator/(const Vector4T<T>& _a, const Vector4T<T>& _b) {
	return Vector4T<T>(_a.x / _b.x, _a.y / _b.y, _a.z / _b.z, _a.w / _b.w);
}

template <typename T>
inline Vector4T<T> operator/(const Vector4T<T>& _v, T _scalar) {
	return Vector4T<T>(_v.x / _scalar, _v.y / _scalar, _v.z / _scalar, _v.w / _scalar);
}

template <typename T>
inline Vector4T<T> operator-(const Vector4T<T>& _v) {
	return Vector4T<T>(-_v.x, -_v.y, -_v.z, -_v.w);
}

template <typename T>
inline Vector4T<T> operator+(const Vector4T<T>& _v) {
	return _v;
}



template <typename T>
void from_json(const nlohmann::json& _j, Vector4T<T>& _v) {
	_v.x = _j.at("x").get<T>();
	_v.y = _j.at("y").get<T>();
	_v.z = _j.at("z").get<T>();
	_v.w = _j.at("w").get<T>();
}

template <typename T>
void to_json(nlohmann::json& _j, const Vector4T<T>& _v) {
	_j = nlohmann::json{
		{ "x", _v.x },
		{ "y", _v.y },
		{ "z", _v.z },
		{ "w", _v.w }
	};
}

}