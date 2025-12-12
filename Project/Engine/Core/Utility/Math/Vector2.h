#pragma once

/// std
#include <format>

/// externals
#include <nlohmann/json.hpp>

/// //////////////////////////////////////////////////
/// 2次元ベクトル
/// //////////////////////////////////////////////////
namespace ONEngine {

struct Vector2 final {
	Vector2();
	Vector2(float _x, float _y);
	Vector2(const Vector2&) = default;
	Vector2(Vector2&&) = default;


	float x, y;



	/// @brief ベクトルの長さを返す
	/// @return ベクトルの長さ
	float Length() const;

	/// @brief 正規化ベクトルを返す
	/// @return 正規化ベクトル
	Vector2 Normalize() const;

	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Vector2 kUp;		//- ( 0,  1 )
	static const Vector2 kDown;		//- ( 0, -1 )
	static const Vector2 kLeft;		//- (-1,  0 )
	static const Vector2 kRight;	//- ( 1,  0 )
	static const Vector2 kOne;		//- ( 1,  1 )
	static const Vector2 kZero;		//- ( 0,  0 )

	static const Vector2 kHD;
	static const Vector2 kFullHD;

	/// ===================================================
	/// public : static methos
	/// ===================================================

	/// @brief 内積の計算
	/// @param _v1 ベクトル1
	/// @param _v2 ベクトル2
	/// @return 内積の値
	static float Dot(const Vector2& _v1, const Vector2& _v2);

	/// @brief ベクトルの線形補完
	/// @param _v1 ベクトル1
	/// @param _v2 ベクトル2
	/// @param _t 媒介変数
	/// @return _tで補完されたベクトル
	static Vector2 Lerp(const Vector2& _v1, const Vector2& _v2, float _t);

	/// @brief _pointが_minと_maxの内側にあるか判定する
	/// @param _point チェックする点	
	/// @param _min 最小値
	/// @param _max 最大値
	/// @return true: 内側にある / false: 内側にない
	static bool Inside(const Vector2& _point, const Vector2& _min, const Vector2& _max);

	/// ===================================================
	/// public : operator overload
	/// ===================================================

	inline Vector2& operator= (const Vector2& _other);
	inline Vector2& operator+= (const Vector2& _other);
	inline Vector2& operator-= (const Vector2& _other);
	inline Vector2& operator/= (const Vector2& _other);
	inline Vector2& operator*= (const Vector2& _other);

	inline Vector2& operator/=(float _scaler);
	inline Vector2& operator*=(float _scaler);

	inline Vector2 operator-();
	inline Vector2 operator+();

};


/// ===================================================
/// 演算子オーバーロード
/// ===================================================
#pragma region operator overload

inline Vector2 operator+(const Vector2& _v1, const Vector2& _v2) {
	return {
		_v1.x + _v2.x,
		_v1.y + _v2.y
	};
}

inline Vector2 operator-(const Vector2& _v1, const Vector2& _v2) {
	return {
		_v1.x - _v2.x,
		_v1.y - _v2.y
	};
}

inline Vector2 operator/(const Vector2& _v1, const Vector2& _v2) {
	return {
		_v1.x / _v2.x,
		_v1.y / _v2.y
	};
}

inline Vector2 operator/(const Vector2& _v, float _scaler) {
	return {
		_v.x / _scaler,
		_v.y / _scaler
	};
}

inline Vector2 operator*(const Vector2& _v1, const Vector2& _v2) {
	return {
		_v1.x * _v2.x,
		_v1.y * _v2.y
	};
}

inline Vector2 operator*(const Vector2& _v, float _scaler) {
	return {
		_v.x * _scaler,
		_v.y * _scaler
	};
}

inline Vector2 operator*(float _scaler, const Vector2& _v) {
	return _v * _scaler;
}

inline bool operator!=(const Vector2& _v1, const Vector2& _v2) {
	return _v1.x != _v2.x || _v1.y != _v2.y;
}

inline bool operator==(const Vector2& _v1, const Vector2& _v2) {
	return  _v1.x == _v2.x && _v1.y == _v2.y;
}


/// ===================================================
/// Vector2 : operator overload
/// ===================================================
inline Vector2& Vector2::operator=(const Vector2& _other) {
	this->x = _other.x;
	this->y = _other.y;
	return *this;
}

inline Vector2& Vector2::operator+=(const Vector2& _other) {
	(*this) = (*this) + _other;
	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& _other) {
	(*this) = (*this) - _other;
	return *this;
}

inline Vector2& Vector2::operator/=(const Vector2& _other) {
	(*this) = (*this) / _other;
	return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& _other) {
	(*this) = (*this) * _other;
	return *this;
}

inline Vector2& Vector2::operator/=(float _scale) {
	(*this) = (*this) / _scale;
	return *this;
}

inline Vector2& Vector2::operator*=(float _scale) {
	(*this) = (*this) * _scale;
	return *this;
}

inline Vector2 Vector2::operator-() {
	return (*this) * -1.0f;
}

inline Vector2 Vector2::operator+() {
	return (*this);
}

#pragma endregion

/// json 変換
void from_json(const nlohmann::json& _j, Vector2& _v);
void to_json(nlohmann::json& _j, const Vector2& _v);

} /// ONEngine

template <>
struct std::formatter<ONEngine::Vector2> {
	constexpr auto parse(format_parse_context& _ctx) {
		return _ctx.begin();  // フォーマットの詳細設定は今回は無視
	}

	template <typename FormatContext>
	auto format(const ONEngine::Vector2& _vec, FormatContext& _ctx) const {
		return std::format_to(_ctx.out(), "({}, {})", _vec.x, _vec.y);
	}
};




