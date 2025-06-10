#pragma once

#include <format>

class Vector3;

/// ===================================================
/// 2次元ベクトル
/// ===================================================
class Vector2 final {
public:

	Vector2();
	Vector2(float x, float y);
	Vector2(const Vector2&) = default;
	Vector2(Vector2&&) = default;

public:

	float x, y;


public:

	float Length() const;
	Vector2 Normalize() const;

public:

	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Vector2 kUp;		//- ( 0,  1 )
	static const Vector2 kDown;		//- ( 0, -1 )
	static const Vector2 kLeft;		//- (-1,  0 )
	static const Vector2 kRight;	//- ( 1,  0 )
	static const Vector2 kOne;		//- ( 1,  1 )
	static const Vector2 kZero;		//- ( 0,  0 )

public:

	/// ===================================================
	/// public : static methos
	/// ===================================================

	static float Dot(const Vector2& v1, const Vector2& v2);

	static Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

public:

	/// ===================================================
	/// public : operator overload
	/// ===================================================

	inline Vector2& operator= (const Vector2& other);
	inline Vector2& operator+= (const Vector2& other);
	inline Vector2& operator-= (const Vector2& other);
	inline Vector2& operator/= (const Vector2& other);
	inline Vector2& operator*= (const Vector2& other);

	inline Vector2& operator/=(float scaler);
	inline Vector2& operator*=(float scaler);

	inline Vector2 operator-();
	inline Vector2 operator+();

};


/// ===================================================
/// 演算子オーバーロード
/// ===================================================
#pragma region operator overload

inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	return {
		v1.x + v2.x,
		v1.y + v2.y
	};
}


inline Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	return {
		v1.x - v2.x,
		v1.y - v2.y
	};
}


inline Vector2 operator/(const Vector2& v1, const Vector2& v2) {
	return {
		v1.x / v2.x,
		v1.y / v2.y
	};
}


inline Vector2 operator/(const Vector2& v, float scaler) {
	return {
		v.x / scaler,
		v.y / scaler
	};
}


inline Vector2 operator*(const Vector2& v1, const Vector2& v2) {
	return {
		v1.x * v2.x,
		v1.y * v2.y
	};
}


inline Vector2 operator*(const Vector2& v, float scaler) {
	return {
		v.x * scaler,
		v.y * scaler
	};
}


inline Vector2 operator*(float scaler, const Vector2& v) {
	return v * scaler;
}


inline bool operator!=(const Vector2& v1, const Vector2& v2) {
	return v1.x != v2.x || v1.y != v2.y;
}


inline bool operator==(const Vector2& v1, const Vector2& v2) {
	return  v1.x == v2.x && v1.y == v2.y;
}


/// ===================================================
/// Vector2 : operator overload
/// ===================================================

inline Vector2& Vector2::operator=(const Vector2& other) {
	this->x = other.x;
	this->y = other.y;
	return *this;
}


inline Vector2& Vector2::operator+=(const Vector2& other) {
	(*this) = (*this) + other;
	return *this;
}


inline Vector2& Vector2::operator-=(const Vector2& other) {
	(*this) = (*this) - other;
	return *this;
}


inline Vector2& Vector2::operator/=(const Vector2& other) {
	(*this) = (*this) / other;
	return *this;
}


inline Vector2& Vector2::operator*=(const Vector2& other) {
	(*this) = (*this) * other;
	return *this;
}


inline Vector2& Vector2::operator/=(float scale) {
	(*this) = (*this) / scale;
	return *this;
}


inline Vector2& Vector2::operator*=(float scale) {
	(*this) = (*this) * scale;
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



using Vec2 = Vector2;