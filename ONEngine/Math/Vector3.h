#pragma once


#include <vector>

class Vector2;

/// ===================================================
/// 3次元ベクトル
/// ===================================================
class Vector3 final {
public:

	Vector3();
	Vector3(float x, float y, float z);
	Vector3(const Vector2& v, float z);
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
	static const Vector3 kFront;	// ( 0,  0, -1 )
	static const Vector3 kBack;		// ( 0,  0,  1 )

	static const Vector3 kOne;		// ( 1,  1,  1 )

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	float Len() const;

	Vector3 Normalize() const;


	/// ===================================================
	/// public : static methods
	/// ===================================================

	static float Length(const Vector3& v);
	static Vector3 Normalize(const Vector3& v);

	static Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

	static Vector3 Cross(const Vector3& v1, const Vector3& v2);
	static float Dot(const Vector3& v1, const Vector3& v2);

	static Vector3 MaxDotVector(const Vector3& direction, const std::vector<Vector3>& vertices);
	static Vector3 MinDotVector(const Vector3& direction, const std::vector<Vector3>& vertices);

public:

	/// ===================================================
	/// public : operator overload
	/// ===================================================

	inline Vector3& operator= (const Vector3& other);
	inline Vector3& operator+= (const Vector3& other);
	inline Vector3& operator-= (const Vector3& other);
	inline Vector3& operator/= (const Vector3& other);
	inline Vector3& operator*= (const Vector3& other);

	inline Vector3& operator/= (float scaler);
	inline Vector3& operator*= (float scaler);

	inline Vector3 operator- () const;
	inline Vector3 operator+ () const;

};



/// ===================================================
/// 演算子オーバーロード
/// ===================================================
#pragma region operator overload

inline Vector3 operator+ (const Vector3& v1, const Vector3& v2) {
	return {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z
	};
}


inline Vector3 operator- (const Vector3& v1, const Vector3& v2) {
	return {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z
	};
}


inline Vector3 operator/ (const Vector3& v1, const Vector3& v2) {
	return {
		v1.x / v2.x,
		v1.y / v2.y,
		v1.z / v2.z
	};
}


inline Vector3 operator/ (const Vector3& v, float scaler) {
	return {
		v.x / scaler,
		v.y / scaler,
		v.z / scaler
	};
}


inline Vector3 operator* (const Vector3& v1, const Vector3& v2) {
	return {
		v1.x * v2.x,
		v1.y * v2.y,
		v1.z * v2.z
	};
}


inline Vector3 operator* (const Vector3& v, float scaler) {
	return {
		v.x * scaler,
		v.y * scaler,
		v.z * scaler
	};
}


inline Vector3 operator* (float scaler, const Vector3& v) {
	return v * scaler;
}


inline bool operator!=(const Vector3& v1, const Vector3& v2) {
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}

inline bool operator==(const Vector3& v1, const Vector3& v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}


/// ===================================================
/// Vector3 : operator overload
/// ===================================================

inline Vector3& Vector3::operator= (const Vector3& other) {
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;
	return *this;
}


inline Vector3& Vector3::operator+= (const Vector3& other) {
	(*this) = *this + other;
	return *this;
}


inline Vector3& Vector3::operator-= (const Vector3& other) {
	(*this) = *this - other;
	return *this;
}


inline Vector3& Vector3::operator/= (const Vector3& other) {
	(*this) = *this / other;
	return *this;
}


inline Vector3& Vector3::operator*= (const Vector3& other) {
	(*this) = *this * other;
	return *this;
}


inline Vector3& Vector3::operator/= (float scaler) {
	(*this) = *this / scaler;
	return *this;
}


inline Vector3& Vector3::operator*= (float scaler) {
	(*this) = *this * scaler;
	return *this;
}


inline Vector3 Vector3::operator-() const {
	return (*this) * -1.0f;
}


inline Vector3 Vector3::operator+() const {
	return (*this);
}

#pragma endregion


using Vec3 = Vector3;