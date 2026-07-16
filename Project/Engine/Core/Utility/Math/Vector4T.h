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

	/**
	 * @brief デフォルトコンストラクタ。零ベクトルで初期化します。
	 */
	Vector4T() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)), w(static_cast<T>(0)) {}

	/**
	 * @brief 各要素の座標値を指定して初期化するコンストラクタ。
	 * @param _x X座標（または赤）
	 * @param _y Y座標（または緑）
	 * @param _z Z座標（または青）
	 * @param _w W座標（またはアルファ）
	 */
	Vector4T(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4T(const Vector4T&) = default;
	Vector4T(Vector4T&&) = default;

	Vector4T& operator=(const Vector4T&) = default;
	Vector4T& operator=(Vector4T&&) = default;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	/**
	 * @brief ベクトルの長さ（ノルム）を取得します。
	 * @param _v 対象のベクトル
	 * @return ベクトルの長さ
	 */
	static T Length(const Vector4T<T>& _v) {
		return static_cast<T>(std::sqrt(_v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w));
	}

	/**
	 * @brief ベクトルの長さの二乗を取得します。sqrt計算を回避したい場合に使用します。
	 * @param _v 対象のベクトル
	 * @return ベクトルの長さの二乗
	 */
	static T LengthSquared(const Vector4T<T>& _v) {
		return _v.x * _v.x + _v.y * _v.y + _v.z * _v.z + _v.w * _v.w;
	}

	/**
	 * @brief ベクトルの正規化（長さを1に変換）を行います。
	 * @param _v 対象のベクトル
	 * @return 正規化されたベクトル。長さが0の場合は零ベクトルを返します。
	 */
	static Vector4T<T> Normalize(const Vector4T<T>& _v) {
		T length = Length(_v);
		if (length == static_cast<T>(0)) {
			return Vector4T<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		}
		return Vector4T<T>(_v.x / length, _v.y / length, _v.z / length, _v.w / length);
	}

	/**
	 * @brief 2つのベクトルの内積を計算します。
	 * @param _a ベクトルA
	 * @param _b ベクトルB
	 * @return 内積（スカラー）
	 */
	static T Dot(const Vector4T<T>& _a, const Vector4T<T>& _b) {
		return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z + _a.w * _b.w;
	}

	/**
	 * @brief 2つのベクトルの線形補間（Lerp）を行います。
	 * @param _a 補間開始ベクトル（t=0）
	 * @param _b 補間終了ベクトル（t=1）
	 * @param _t 補間係数
	 * @return 補間されたベクトル
	 */
	static Vector4T<T> Lerp(const Vector4T<T>& _a, const Vector4T<T>& _b, T _t) {
		return Vector4T<T>(
			_a.x + (_b.x - _a.x) * _t,
			_a.y + (_b.y - _a.y) * _t,
			_a.z + (_b.z - _a.z) * _t,
			_a.w + (_b.w - _a.w) * _t
		);
	}

	/**
	 * @brief 2つのベクトルの球面線形補間（Slerp）を行います。4次元クォータニオン回転などの補間に適しています。
	 * @param _a 補間開始ベクトル
	 * @param _b 補間終了ベクトル
	 * @param _t 補間係数
	 * @return 補間されたベクトル
	 */
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
	Vector4T<T> Normalize() const {
		return Normalize(*this);
	}

	/**
	 * @brief 別のベクトルとの内積を計算します。
	 * @param _other 対象のベクトル
	 * @return 内積値
	 */
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



/**
 * @brief jsonオブジェクトからVector4T構造体へのデシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _v 変換先のVector4T構造体の参照
 */
template <typename T>
void from_json(const nlohmann::json& _j, Vector4T<T>& _v) {
	_v.x = _j.at("x").get<T>();
	_v.y = _j.at("y").get<T>();
	_v.z = _j.at("z").get<T>();
	_v.w = _j.at("w").get<T>();
}

/**
 * @brief Vector4T構造体からjsonオブジェクトへのシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _v 変換元のVector4T構造体
 */
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