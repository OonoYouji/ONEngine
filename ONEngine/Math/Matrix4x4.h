#pragma once

#include <cmath>
#include <DirectXMath.h>


using namespace DirectX;


class Vector3;
class Vector4;

/// ===================================================
/// 4x4行列
/// ===================================================
class Matrix4x4 final {
public:

	Matrix4x4();
	Matrix4x4(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4);
	Matrix4x4(
		float aa, float ab, float ac, float ad,
		float ba, float bb, float bc, float bd,
		float ca, float cb, float cc, float cd,
		float da, float db, float dc, float dd
	);
	Matrix4x4(const Matrix4x4&) = default;
	Matrix4x4(Matrix4x4&&) = default;
	Matrix4x4(const XMMATRIX& xm);


public:

	/// ===================================================
	/// public : operator
	/// ===================================================

	Matrix4x4 Inverse() const;


public:
	
	/// ===================================================
	/// public : objects
	/// ===================================================

	float m[4][4];



public:

	/// ===================================================
	/// public : static objects
	/// ===================================================

	/// <summary>
	/// >> 単位行列
	/// </summary>
	static const Matrix4x4 kIdentity;


public:

	/// ===================================================
	/// public : static methods
	/// ===================================================

	static Matrix4x4 MakeScale(const Vector3& v);
	static Matrix4x4 MakeRotateX(float theta);
	static Matrix4x4 MakeRotateY(float theta);
	static Matrix4x4 MakeRotateZ(float theta);
	static Matrix4x4 MakeRotate(const Vector3& v);
	static Matrix4x4 MakeTranslate(const Vector3& v);
	static Matrix4x4 MakeAffine(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	static Matrix4x4 MakeInverse(const Matrix4x4& m);

	static Matrix4x4 MakeOrthographicMatrix(float l, float t, float r, float b, float nearZ, float farZ);
	static Matrix4x4 MakeOrthographicMatrix(float width, float height, float nearZ, float farZ);

	static Matrix4x4 MakeViewport(float top, float left, float width, float height, float minD, float maxD);

	static Vector3 Transform(const Vector3& v, const Matrix4x4& m);
	static Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

public:

	/// ===================================================
	/// public : operator
	/// ===================================================

	inline Matrix4x4& operator= (const Matrix4x4& other);

	inline Matrix4x4& operator*= (const Matrix4x4& other);

};



/// ===================================================
/// 無記名名前空間 : このヘッダで使用する関数の定義
/// ===================================================
namespace {
	inline XMMATRIX ConvertToXMMATRIX(const Matrix4x4& matrix) {
		return XMMATRIX(
			matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
			matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
			matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
			matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]
		);
	}


	inline Matrix4x4 ConvertToMatrix4x4(const XMMATRIX& xmMatrix) {
		Matrix4x4 result;
		XMFLOAT4X4 tempMatrix;
		XMStoreFloat4x4(&tempMatrix, xmMatrix);

		for(int i = 0; i < 4; ++i) {
			for(int j = 0; j < 4; ++j) {
				result.m[i][j] = tempMatrix.m[i][j];
			}
		}
		return result;
	}

}


/// ===================================================
/// 演算子オーバーロード
/// ===================================================

#pragma region operator overload

inline Matrix4x4 operator* (const Matrix4x4& m1, const Matrix4x4& m2) {
	return Matrix4x4(ConvertToXMMATRIX(m1) * ConvertToXMMATRIX(m2));
}

/// ===================================================
/// Matrix4x4 : operator overload
/// ===================================================

inline Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other) {
	for(uint8_t r = 0u; r < 4; ++r) {
		for(uint8_t c = 0u; c < 4; ++c) {
			this->m[r][c] = other.m[r][c];
		}
	}
	return *this;
}

inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) {
	(*this) = (*this) * other;
	return *this;
}

#pragma endregion


using Mat4 = Matrix4x4;



