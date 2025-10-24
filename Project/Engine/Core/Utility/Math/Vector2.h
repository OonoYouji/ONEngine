#pragma once

/// std
#include <format>

/// externals
#include <nlohmann/json.hpp>

/// //////////////////////////////////////////////////
/// 2次元ベクトル
/// //////////////////////////////////////////////////
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


template <>
struct std::formatter<Vector2> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();  // フォーマットの詳細設定は今回は無視
	}

	template <typename FormatContext>
	auto format(const Vector2& vec, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({}, {})", vec.x, vec.y);
	}
};



/// json 変換
void from_json(const nlohmann::json& _j, Vector2& _v);
void to_json(nlohmann::json& _j, const Vector2& _v);
