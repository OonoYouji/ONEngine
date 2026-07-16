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

	/**
	 * @brief デフォルトコンストラクタ。零ベクトルで初期化します。
	 */
	Vector3T() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) {}

	/**
	 * @brief 各要素の座標値を指定して初期化するコンストラクタ。
	 * @param _x X座標値
	 * @param _y Y座標値
	 * @param _z Z座標値
	 */
	Vector3T(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	Vector3T(const Vector3T&) = default;
	Vector3T(Vector3T&&) = default;

	Vector3T& operator=(const Vector3T&) = default;
	Vector3T& operator=(Vector3T&&) = default;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	/**
	 * @brief ベクトルの長さ（ノルム）を取得します。
	 * @param _v 対象のベクトル
	 * @return ベクトルの長さ
	 */
	static T Length(const Vector3T<T>& _v) {
		return static_cast<T>(std::sqrt(_v.x * _v.x + _v.y * _v.y + _v.z * _v.z));
	}

	/**
	 * @brief ベクトルの長さの二乗を取得します。sqrt計算を回避したい場合に使用します。
	 * @param _v 対象のベクトル
	 * @return ベクトルの長さの二乗
	 */
	static T LengthSquared(const Vector3T<T>& _v) {
		return _v.x * _v.x + _v.y * _v.y + _v.z * _v.z;
	}

	/**
	 * @brief ベクトルの正規化（長さを1に変換）を行います。
	 * @param _v 対象のベクトル
	 * @return 正規化されたベクトル。長さが0の場合は零ベクトルを返します。
	 */
	static Vector3T<T> Normalize(const Vector3T<T>& _v) {
		T length = Length(_v);
		if (length == static_cast<T>(0)) {
			return Vector3T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		}
		return Vector3T<T>(_v.x / length, _v.y / length, _v.z / length);
	}

	/**
	 * @brief 2つのベクトルの内積を計算します。
	 * @param _a ベクトルA
	 * @param _b ベクトルB
	 * @return 内積（スカラー）
	 */
	static T Dot(const Vector3T<T>& _a, const Vector3T<T>& _b) {
		return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
	}

	/**
	 * @brief 2つのベクトルの外積（クロス積）を計算します。3D空間で直交するベクトルを求めます。
	 * @param _a ベクトルA
	 * @param _b ベクトルB
	 * @return 外積ベクトル
	 */
	static Vector3T<T> Cross(const Vector3T<T>& _a, const Vector3T<T>& _b) {
		return Vector3T<T>(
			_a.y * _b.z - _a.z * _b.y,
			_a.z * _b.x - _a.x * _b.z,
			_a.x * _b.y - _a.y * _b.x
		);
	}

	/**
	 * @brief 2つのベクトルの線形補間（Lerp）を行います。
	 * @param _a 補間開始ベクトル（t=0）
	 * @param _b 補間終了ベクトル（t=1）
	 * @param _t 補間係数
	 * @return 補間されたベクトル
	 */
	static Vector3T<T> Lerp(const Vector3T<T>& _a, const Vector3T<T>& _b, T _t) {
		return Vector3T<T>(
			_a.x + (_b.x - _a.x) * _t,
			_a.y + (_b.y - _a.y) * _t,
			_a.z + (_b.z - _a.z) * _t
		);
	}

	/**
	 * @brief 2つのベクトルの球面線形補間（Slerp）を行います。3D回転や方向の補間に適しています。
	 * @param _a 補間開始ベクトル
	 * @param _b 補間終了ベクトル
	 * @param _t 補間係数
	 * @return 補間されたベクトル
	 */
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

	/**
	 * @brief 自身のベクトルの長さ（ノルム）を取得します。
	 * @return ベクトルの長さ
	 */
	T Length() const {
		return Length(*this);
	}

	/**
	 * @brief 自身のベクトルの長さの二乗を取得します。
	 * @return ベクトルの長さの二乗
	 */
	T LengthSquared() const {
		return LengthSquared(*this);
	}

	/**
	 * @brief 自身を正規化したベクトルを取得します（自身の内容は変更されません）。
	 * @return 正規化されたベクトル
	 */
	Vector3T<T> Normalize() const {
		return Normalize(*this);
	}

	/**
	 * @brief 別のベクトルとの内積を計算します。
	 * @param _other 対象のベクトル
	 * @return 内積値
	 */
	T Dot(const Vector3T<T>& _other) const {
		return Dot(*this, _other);
	}

	/**
	 * @brief 別のベクトルとの外積を計算します。
	 * @param _other 対象のベクトル
	 * @return 外積ベクトル
	 */
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



/**
 * @brief jsonオブジェクトからVector3T構造体へのデシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _v 変換先のVector3T構造体の参照
 */
template <typename T>
void from_json(const nlohmann::json& _j, Vector3T<T>& _v) {
	_v.x = _j.at("x").get<T>();
	_v.y = _j.at("y").get<T>();
	_v.z = _j.at("z").get<T>();
}

/**
 * @brief Vector3T構造体からjsonオブジェクトへのシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _v 変換元のVector3T構造体
 */
template <typename T>
void to_json(nlohmann::json& _j, const Vector3T<T>& _v) {
	_j = nlohmann::json{
		{ "x", _v.x },
		{ "y", _v.y },
		{ "z", _v.z }
	};
}

}