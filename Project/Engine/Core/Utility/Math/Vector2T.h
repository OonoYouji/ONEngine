#pragma once

/// std
#include <cmath>

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

	static constexpr Vector2T<T> Zero = Vector2T<T>(static_cast<T>(0), static_cast<T>(0));
	static constexpr Vector2T<T> One = Vector2T<T>(static_cast<T>(1), static_cast<T>(1));

	static constexpr Vector2T<T> Left = Vector2T<T>(static_cast<T>(-1), static_cast<T>(0));
	static constexpr Vector2T<T> Right = Vector2T<T>(static_cast<T>(1), static_cast<T>(0));
	static constexpr Vector2T<T> Up = Vector2T<T>(static_cast<T>(0), static_cast<T>(1));
	static constexpr Vector2T<T> Down = Vector2T<T>(static_cast<T>(0), static_cast<T>(-1));

	static constexpr Vector2T<T> Infinity = Vector2T<T>(std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity());
	static constexpr Vector2T<T> NegativeInfinity = Vector2T<T>(-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity());

	static constexpr Vector2T<T> Max = Vector2T<T>(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
	static constexpr Vector2T<T> Min = Vector2T<T>(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest());

	static constexpr Vector2T<T> HD = Vector2T<T>(static_cast<T>(1280), static_cast<T>(720));
	static constexpr Vector2T<T> FHD = Vector2T<T>(static_cast<T>(1920), static_cast<T>(1080));

	static constexpr Vector2T<T> Half = Vector2T<T>(static_cast<T>(0.5), static_cast<T>(0.5));

	/// ===================================================
	/// public : constructors
	/// ===================================================

	Vector2T() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}
	Vector2T(T _x, T _y) : x(_x), y(_y) {}
	Vector2T(const Vector2T&) = default;
	Vector2T(Vector2T&&) = default;
	Vector2T& operator=(const Vector2T&) = default;
	Vector2T& operator=(Vector2T&&) = default;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	/// @brief ベクトルの長さを取得
	/// @param _v ベクトル
	/// @return ベクトルの長さ
	static T Length(const Vector2T<T>& _v) {
		return static_cast<T>(std::sqrt(_v.x * _v.x + _v.y * _v.y));
	}

	/// @brief ベクトルの長さの二乗を取得
	/// @param _v ベクトル
	/// @return ベクトルの長さの二乗
	static T LengthSquared(const Vector2T<T>& _v) {
		return _v.x * _v.x + _v.y * _v.y;
	}

	/// @brief ベクトルの正規化
	/// @param _v ベクトル
	/// @return 正規化されたベクトル
	static Vector2T<T> Normalize(const Vector2T<T>& _v) {
		T length = Length(_v);
		if (length == static_cast<T>(0)) {
			return Vector2T<T>(static_cast<T>(0), static_cast<T>(0));
		}
		return Vector2T<T>(_v.x / length, _v.y / length);
	}

	/// @brief ベクトルの内積を取得
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @return ベクトルの内積
	static T Dot(const Vector2T<T>& _a, const Vector2T<T>& _b) {
		return _a.x * _b.x + _a.y * _b.y;
	}

	/// @brief ベクトルの外積
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @return ベクトルの外積
	static T Cross(const Vector2T<T>& _a, const Vector2T<T>& _b) {
		return _a.x * _b.y - _a.y * _b.x;
	}

	/// @brief ベクトルの線形補完
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @param _t 補間係数
	/// @return 補間されたベクトル
	static Vector2T<T> Lerp(const Vector2T<T>& _a, const Vector2T<T>& _b, T _t) {
		return Vector2T<T>(
			_a.x + (_b.x - _a.x) * _t,
			_a.y + (_b.y - _a.y) * _t
		);
	}

	/// @brief ベクトルの球面線形補完
	/// @param _a ベクトルA
	/// @param _b ベクトルB
	/// @param _t 補間係数
	/// @return 補間されたベクトル
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

	/// @brief ベクトルの長さを取得
	/// @return ベクトルの長さ
	float Length() const {
		return Length(*this);
	}

	/// @brief ベクトルの長さの二乗を取得
	/// @return ベクトルの長さの二乗
	float LengthSquared() const {
		return LengthSquared(*this);
	}

	/// @brief ベクトルの正規化
	/// @return 正規化されたベクトル
	Vector2T<T> Normalize() const {
		return Normalize(*this);
	}

	/// @brief ベクトルの内積を取得
	/// @param _other もう一つのベクトル
	/// @return ベクトルの内積
	float Dot(const Vector2T<T>& _other) const {
		return Dot(*this, _other);
	}

	/// @brief ベクトルの外積を取得
	/// @param _other もう一つのベクトル
	/// @return ベクトルの外積
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



template <typename T>
void from_json(const nlohmann::json& _j, Vector2T<T>& _v) {
	_v.x = _j.at("x").get<T>();
	_v.y = _j.at("y").get<T>();
}

template <typename T>
void to_json(nlohmann::json& _j, const Vector2T<T>& _v) {
	_j = nlohmann::json{
		{ "x", _v.x },
		{ "y", _v.y }
	};
}

}


template <typename T>
struct std::formatter<ONEngine::Vector2T<T>> {
	constexpr auto parse(format_parse_context& _ctx) {
		return _ctx.begin();
	}

	template <typename FormatContext>
	auto format(const ONEngine::Vector2& _vec, FormatContext& _ctx) const {
		return std::format_to(_ctx.out(), "({}, {})", _vec.x, _vec.y);
	}
};