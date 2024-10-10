#pragma once



/// ===================================================
/// 4次元ベクトル
/// ===================================================
class Vector4 final {
public:

	Vector4();
	Vector4(float x, float y, float z, float w);
	Vector4(const Vector4&) = default;
	Vector4(Vector4&&) = default;

public:

	float x, y, z, w;

public:

	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Vector4 k1000;
	static const Vector4 k0100;
	static const Vector4 k0010;
	static const Vector4 k0001;

	static const Vector4 kWhite;
	static const Vector4 kRed;
	static const Vector4 kGreen;
	static const Vector4 kBlue;
	
	
	static Vector4 Lerp(
		const Vector4& start,
		const Vector4& end,
		float t
	);

	static float Lenght(const Vector4& v);
	static Vector4 Normalize(const Vector4& v);

public:

	/// ===================================================
	/// public : operator overload
	/// ===================================================


	inline Vector4& operator= (const Vector4& other) = default;
	inline Vector4& operator+= (const Vector4& other);
	inline Vector4& operator-= (const Vector4& other);
	inline Vector4& operator/= (const Vector4& other);
	inline Vector4& operator*= (const Vector4& other);

	inline Vector4& operator/= (float scaler);
	inline Vector4& operator*= (float scaler);

	inline Vector4 operator- ();
	inline Vector4 operator+ ();


};



/// ===================================================
/// 演算子オーバーロード
/// ===================================================
#pragma region operator overload

inline Vector4 operator+ (const Vector4& v1, const Vector4& v2) {
	return {
		v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z,
		v1.w + v2.w
	};
}


inline Vector4 operator- (const Vector4& v1, const Vector4& v2) {
	return {
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z,
		v1.w - v2.w
	};
}


inline Vector4 operator/ (const Vector4& v1, const Vector4& v2) {
	return {
		v1.x / v2.x,
		v1.y / v2.y,
		v1.z / v2.z,
		v1.w / v2.w
	};
}

inline Vector4 operator/ (const Vector4& v, float scaler) {
	return {
		v.x / scaler,
		v.y / scaler,
		v.z / scaler,
		v.w / scaler
	};
}


inline Vector4 operator* (const Vector4& v1, const Vector4& v2) {
	return {
		v1.x * v2.x,
		v1.y * v2.y,
		v1.z * v2.z,
		v1.w * v2.w
	};
}


inline Vector4 operator* (const Vector4& v, float scaler) {
	return {
		v.x * scaler,
		v.y * scaler,
		v.z * scaler,
		v.w * scaler
	};
}


inline Vector4 operator* (float scaler, const Vector4& v) {
	return v * scaler;
}


inline bool operator!=(const Vector4& v1, const Vector4& v2) {
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
}


inline bool operator==(const Vector4& v1, const Vector4& v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}


/// ===================================================
/// Vector4 : operator overload
/// ===================================================


//inline Vector4& Vector4::operator= (const Vector4& other) {
//	this->x = other.x;
//	this->y = other.y;
//	this->z = other.z;
//	this->w = other.w;
//	return *this;
//}




inline Vector4& Vector4::operator+= (const Vector4& other) {
	(*this) = *this + other;
	return *this;
}


inline Vector4& Vector4::operator-= (const Vector4& other) {
	(*this) = *this - other;
	return *this;
}


inline Vector4& Vector4::operator/= (const Vector4& other) {
	(*this) = *this / other;
	return *this;
}


inline Vector4& Vector4::operator*= (const Vector4& other) {
	(*this) = *this * other;
	return *this;
}


inline Vector4& Vector4::operator/= (float scaler) {
	(*this) = *this / scaler;
	return *this;
}


inline Vector4& Vector4::operator*= (float scaler) {
	(*this) = *this * scaler;
	return *this;
}


inline Vector4 Vector4::operator-() {
	return (*this) * -1.0f;
}


inline Vector4 Vector4::operator+() {
	return (*this);
}

#pragma endregion


using Vec4 = Vector4;