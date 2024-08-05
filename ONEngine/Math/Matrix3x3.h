#pragma once


class Vector3;
class Vector2;
class Matrix4x4;


/// ===================================================
/// 3x3行列
/// ===================================================
class Matrix3x3 final {
public:

	/// ===================================================
	/// public : constractor
	/// ===================================================

	Matrix3x3();
	Matrix3x3(const Matrix3x3&) = default;
	Matrix3x3(Matrix3x3&&) = default;
	Matrix3x3(
		float aa, float ab, float ac,
		float ba, float bb, float bc,
		float ca, float cb, float cc
	);

	Matrix3x3(const Vector3& row1, const Vector3& row2, const Vector3& row3);

public:
	/// ===================================================
	/// public : objects
	/// ===================================================

	float m[3][3];

public:
	/// ===================================================
	/// public : static objects
	/// ===================================================

	static const Matrix3x3 kIdentity;

public:
	/// ===================================================
	/// public : static methods
	/// ===================================================

	static Matrix3x3 MakeScale(const Vector2& scale);
	static Matrix3x3 MakeRotate(float rotate);
	static Matrix3x3 MakeTranslate(const Vector2& translate);

	static Matrix3x3 MakeAffine(const Vector2& scale, float rotate, const Vector2& translate);
	
	static Matrix4x4 CopyMatrix(const Matrix3x3& m);

	static const Matrix3x3& Identity() { return kIdentity; }

public:
	/// ===================================================
	/// public : operator
	/// ===================================================

	inline Matrix3x3& operator= (const Matrix3x3& other);

	inline Matrix3x3 operator*(const Matrix3x3& other) const;
	inline Matrix3x3& operator*=(const Matrix3x3& other);

};




inline Matrix3x3& Matrix3x3::operator=(const Matrix3x3& other) {
	for(int r = 0; r < 3; ++r) {
		for(int c = 0; c < 3; ++c) {
			for(int i = 0; i < 3; ++i) {
				this->m[r][c] = other.m[r][c];
			}
		}
	}
	return *this;
}

inline Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const {
	Matrix3x3 result{};
	for(int r = 0; r < 3; ++r) {
		for(int c = 0; c < 3; ++c) {
			for(int i = 0; i < 3; ++i) {
				result.m[r][c] += this->m[r][i] * other.m[i][c];
			}
		}
	}

	return result;
}

inline Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& other) {
	*this = *this * other;
	return *this;
}
