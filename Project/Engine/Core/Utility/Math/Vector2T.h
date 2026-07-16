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
/// Vector2のテンプレート版
/// ///////////////////////////////////////////////////
template <typename T>
struct Vector2T final {
	/// ===================================================
	/// public : objects
	/// ===================================================

	T x, y;


	/// ===================================================
	/// public : constants
	/// ===================================================

	static const Vector2T<T> Zero;
	static const Vector2T<T> One;

	static const Vector2T<T> Left;
	static const Vector2T<T> Right;
	static const Vector2T<T> Up;
	static const Vector2T<T> Down;

	static const Vector2T<T> Infinity;
	static const Vector2T<T> NegativeInfinity;

	static const Vector2T<T> Max;
	static const Vector2T<T> Min;

	static const Vector2T<T> HD;
	static const Vector2T<T> FHD;

	static const Vector2T<T> Half;


	/// ===================================================
	/// public : constructors
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ。零ベクトルで初期化します。
	 */
	Vector2T() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

	/**
	 * @brief x, y の座標値を明示的に指定して初期化するコンストラクタ。
	 * @param _x X座標値
	 * @param _y Y座標値
	 */
	Vector2T(T _x, T _y) : x(_x), y(_y) {}
	Vector2T(const Vector2T&) = default;
	Vector2T(Vector2T&&) = default;

	Vector2T& operator=(const Vector2T&) = default;
	Vector2T& operator=(Vector2T&&) = default;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	/**
	 * @brief ベクトルの長さ（ノルム）を取得します。
	 * @param _v 対象のベクトル
	 * @return ベクトルの長さ
	 */
	static T Length(const Vector2T<T>& _v) {
		return static_cast<T>(std::sqrt(_v.x * _v.x + _v.y * _v.y));
	}

	/**
	 * @brief ベクトルの長さの二乗を取得します。sqrt計算を回避したい場合に使用します。
	 * @param _v 対象のベクトル
	 * @return ベクトルの長さの二乗
	 */
	static T LengthSquared(const Vector2T<T>& _v) {
		return _v.x * _v.x + _v.y * _v.y;
	}

	/**
	 * @brief ベクトルの正規化（長さを1に変換）を行います。
	 * @param _v 対象のベクトル
	 * @return 正規化されたベクトル。長さが0の場合は零ベクトルを返します。
	 */
	static Vector2T<T> Normalize(const Vector2T<T>& _v) {
		T length = Length(_v);
		if (length == static_cast<T>(0)) {
			return Vector2T<T>(static_cast<T>(0), static_cast<T>(0));
		}
		return Vector2T<T>(_v.x / length, _v.y / length);
	}

	/**
	 * @brief 2つのベクトルの内積を計算します。
	 * @param _a ベクトルA
	 * @param _b ベクトルB
	 * @return 内積（スカラー）
	 */
	static T Dot(const Vector2T<T>& _a, const Vector2T<T>& _b) {
		return _a.x * _b.x + _a.y * _b.y;
	}

	/**
	 * @brief 2つのベクトルの外積（2Dにおける外積のZ成分相当）を計算します。
	 * @param _a ベクトルA
	 * @param _b ベクトルB
	 * @return 外積の大きさ
	 */
	static T Cross(const Vector2T<T>& _a, const Vector2T<T>& _b) {
		return _a.x * _b.y - _a.y * _b.x;
	}

	/**
	 * @brief 2つのベクトルの線形補間（Lerp）を行います。
	 * @param _a 補間開始ベクトル（t=0）
	 * @param _b 補間終了ベクトル（t=1）
	 * @param _t 補間係数
	 * @return 補間されたベクトル
	 */
	static Vector2T<T> Lerp(const Vector2T<T>& _a, const Vector2T<T>& _b, T _t) {
		return Vector2T<T>(
			_a.x + (_b.x - _a.x) * _t,
			_a.y + (_b.y - _a.y) * _t
		);
	}

	/**
	 * @brief 2つのベクトルの球面線形補間（Slerp）を行います。方向の補間に適しています。
	 * @param _a 補間開始ベクトル
	 * @param _b 補間終了ベクトル
	 * @param _t 補間係数
	 * @return 補間されたベクトル
	 */
	static Vector2T<T> Slerp(const Vector2T<T>& _a, const Vector2T<T>& _b, T _t) {
		T dot = Dot(Normalize(_a), Normalize(_b));
		dot = std::clamp(dot, static_cast<T>(-1), static_cast<T>(1));
		T theta = std::acos(dot) * _t;
		Vector2T<T> relativeVec = _b - _a * dot;
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
	float Length() const {
		return Length(*this);
	}

	/**
	 * @brief 自身のベクトルの長さの二乗を取得します。
	 * @return ベクトルの長さの二乗
	 */
	float LengthSquared() const {
		return LengthSquared(*this);
	}

	/**
	 * @brief 自身を正規化したベクトルを取得します（自身の内容は変更されません）。
	 * @return 正規化されたベクトル
	 */
	Vector2T<T> Normalize() const {
		return Normalize(*this);
	}

	/**
	 * @brief 別のベクトルとの内積を計算します。
	 * @param _other 対象のベクトル
	 * @return 内積値
	 */
	float Dot(const Vector2T<T>& _other) const {
		return Dot(*this, _other);
	}

	/**
	 * @brief 別のベクトルとの外積を計算します。
	 * @param _other 対象のベクトル
	 * @return 外積値
	 */
	float Cross(const Vector2T<T>& _other) const {
		return Cross(*this, _other);
	}


	/// ===================================================
	/// public : operators
	/// ===================================================

	Vector2T<T>& operator+=(const Vector2T<T>& _other) {
		x += _other.x;
		y += _other.y;
		return *this;
	}

	Vector2T<T>& operator-=(const Vector2T<T>& _other) {
		x -= _other.x;
		y -= _other.y;
		return *this;
	}

	Vector2T<T>& operator*=(const Vector2T<T>& _other) {
		x *= _other.x;
		y *= _other.y;
		return *this;
	}

	Vector2T<T>& operator*=(T _scalar) {
		x *= _scalar;
		y *= _scalar;
		return *this;
	}

	Vector2T<T>& operator/=(const Vector2T<T>& _other) {
		x /= _other.x;
		y /= _other.y;
		return *this;
	}

	Vector2T<T>& operator/=(T _scalar) {
		x /= _scalar;
		y /= _scalar;
		return *this;
	}


};


// クラス外で定義
template <typename T>
const Vector2T<T> Vector2T<T>::Zero = Vector2T<T>(static_cast<T>(0), static_cast<T>(0));

template <typename T>
const Vector2T<T> Vector2T<T>::One = Vector2T<T>(static_cast<T>(1), static_cast<T>(1));

template <typename T>
const Vector2T<T> Vector2T<T>::Left = Vector2T<T>(static_cast<T>(-1), static_cast<T>(0));

template <typename T>
const Vector2T<T> Vector2T<T>::Right = Vector2T<T>(static_cast<T>(1), static_cast<T>(0));

template <typename T>
const Vector2T<T> Vector2T<T>::Up = Vector2T<T>(static_cast<T>(0), static_cast<T>(1));

template <typename T>
const Vector2T<T> Vector2T<T>::Down = Vector2T<T>(static_cast<T>(0), static_cast<T>(-1));

template <typename T>
const Vector2T<T> Vector2T<T>::Infinity = Vector2T<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());

template <typename T>
const Vector2T<T> Vector2T<T>::NegativeInfinity = Vector2T<T>(-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity());

template <typename T>
const Vector2T<T> Vector2T<T>::Max = Vector2T<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());

template <typename T>
const Vector2T<T> Vector2T<T>::Min = Vector2T<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest());

template <typename T>
const Vector2T<T> Vector2T<T>::HD = Vector2T<T>(static_cast<T>(1280), static_cast<T>(720));

template <typename T>
const Vector2T<T> Vector2T<T>::FHD = Vector2T<T>(static_cast<T>(1920), static_cast<T>(1080));

template <typename T>
const Vector2T<T> Vector2T<T>::Half = Vector2T<T>(static_cast<T>(0.5), static_cast<T>(0.5));


/// ///////////////////////////////////////////////////
/// operator
/// ///////////////////////////////////////////////////

template <typename T>
inline Vector2T<T> operator+(const Vector2T<T>& _a, const Vector2T<T>& _b) {
	return Vector2T<T>(_a.x + _b.x, _a.y + _b.y);
}

template <typename T>
inline Vector2T<T> operator-(const Vector2T<T>& _a, const Vector2T<T>& _b) {
	return Vector2T<T>(_a.x - _b.x, _a.y - _b.y);
}

template <typename T>
inline Vector2T<T> operator*(const Vector2T<T>& _a, const Vector2T<T>& _b) {
	return Vector2T<T>(_a.x * _b.x, _a.y * _b.y);
}

template <typename T>
inline Vector2T<T> operator*(const Vector2T<T>& _v, T _scalar) {
	return Vector2T<T>(_v.x * _scalar, _v.y * _scalar);
}

template <typename T>
inline Vector2T<T> operator*(T _scalar, const Vector2T<T>& _v) {
	return Vector2T<T>(_v.x * _scalar, _v.y * _scalar);
}

template <typename T>
inline Vector2T<T> operator/(const Vector2T<T>& _a, const Vector2T<T>& _b) {
	return Vector2T<T>(_a.x / _b.x, _a.y / _b.y);
}

template <typename T>
inline Vector2T<T> operator/(const Vector2T<T>& _v, T _scalar) {
	return Vector2T<T>(_v.x / _scalar, _v.y / _scalar);
}

template <typename T>
inline Vector2T<T> operator-(const Vector2T<T>& _v) {
	return Vector2T<T>(-_v.x, -_v.y);
}

template <typename T>
inline Vector2T<T> operator+(const Vector2T<T>& _v) {
	return _v;
}



/**
 * @brief jsonオブジェクトからVector2T構造体へのデシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _v 変換先のVector2T構造体の参照
 */
template <typename T>
void from_json(const nlohmann::json& _j, Vector2T<T>& _v) {
	_v.x = _j.at("x").get<T>();
	_v.y = _j.at("y").get<T>();
}

/**
 * @brief Vector2T構造体からjsonオブジェクトへのシリアライズを行います。
 * @param _j jsonオブジェクト
 * @param _v 変換元のVector2T構造体
 */
template <typename T>
void to_json(nlohmann::json& _j, const Vector2T<T>& _v) {
	_j = nlohmann::json{
		{ "x", _v.x },
		{ "y", _v.y }
	};
}

}