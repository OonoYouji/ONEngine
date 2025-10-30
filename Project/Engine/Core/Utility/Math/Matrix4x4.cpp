#include "Matrix4x4.h"

/// std
#include <cmath>

/// engine
#include "Quaternion.h"


using namespace DirectX;


/// @brief 単位行列の定義
const Matrix4x4 Matrix4x4::kIdentity = Matrix4x4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);


/// ===================================================
/// public : constructer
/// ===================================================

Matrix4x4::Matrix4x4() {
	*this = kIdentity;
}

Matrix4x4::Matrix4x4(const Matrix4x4& _matrix) {
	for (size_t r = 0; r < 4; r++) {
		for (size_t c = 0; c < 4; c++) {
			m[r][c] = _matrix.m[r][c];
		}
	}
}

Matrix4x4::Matrix4x4(const float _matrix[4][4]) {
	for (size_t r = 0; r < 4; r++) {
		for (size_t c = 0; c < 4; c++) {
			m[r][c] = _matrix[r][c];
		}
	}
}

Matrix4x4::Matrix4x4(float _m00, float _m01, float _m02, float _m03, float _m10, float _m11, float _m12, float _m13, float _m20, float _m21, float _m22, float _m23, float _m30, float _m31, float _m32, float _m33) {
	m[0][0] = _m00;
	m[0][1] = _m01;
	m[0][2] = _m02;
	m[0][3] = _m03;

	m[1][0] = _m10;
	m[1][1] = _m11;
	m[1][2] = _m12;
	m[1][3] = _m13;
	
	m[2][0] = _m20;
	m[2][1] = _m21;
	m[2][2] = _m22;
	m[2][3] = _m23;
	
	m[3][0] = _m30;
	m[3][1] = _m31;
	m[3][2] = _m32;
	m[3][3] = _m33;
}



/// ===================================================
/// public : static methods
/// ===================================================

Matrix4x4 Matrix4x4::MakeScale(const Vector3& _v) {
	return Matrix4x4(
		_v.x, 0.0f, 0.0f, 0.0f,
		0.0f, _v.y, 0.0f, 0.0f,
		0.0f, 0.0f, _v.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Matrix4x4 Matrix4x4::MakeRotateX(float _angle) {
	return Matrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,  std::cos(_angle), std::sin(_angle), 0.0f,
		0.0f, -std::sin(_angle), std::cos(_angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Matrix4x4 Matrix4x4::MakeRotateY(float _angle) {
	return Matrix4x4(
		std::cos(_angle), 0.0f, -std::sin(_angle), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sin(_angle), 0.0f,  std::cos(_angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Matrix4x4 Matrix4x4::MakeRotateZ(float _angle) {
	return Matrix4x4(
		 std::cos(_angle), std::sin(_angle), 0.0f, 0.0f,
		-std::sin(_angle), std::cos(_angle), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Matrix4x4 Matrix4x4::MakeRotate(const Vector3& _v) {
	Matrix4x4&& x = MakeRotateX(_v.x);
	Matrix4x4&& y = MakeRotateY(_v.y);
	Matrix4x4&& z = MakeRotateZ(_v.z);

	Matrix4x4&& result = x * y * z;

	return result;
}

Matrix4x4 Matrix4x4::MakeRotate(const Quaternion& _q) {
	if (Quaternion::Length(_q) == 0.0f) {
		return kIdentity;
	}
	Matrix4x4 result{};

	float ww = _q.w * _q.w;
	float xx = _q.x * _q.x;
	float yy = _q.y * _q.y;
	float zz = _q.z * _q.z;
	float wx = _q.w * _q.x;
	float wy = _q.w * _q.y;
	float wz = _q.w * _q.z;
	float xy = _q.x * _q.y;
	float xz = _q.x * _q.z;
	float yz = _q.y * _q.z;

	result.m[0][0] = ww + xx - yy - zz;
	result.m[0][1] = 2 * (xy + wz);
	result.m[0][2] = 2 * (xz - wy);

	result.m[1][0] = 2 * (xy - wz);
	result.m[1][1] = ww - xx + yy - zz;
	result.m[1][2] = 2 * (yz + wx);

	result.m[2][0] = 2 * (xz + wy);
	result.m[2][1] = 2 * (yz - wx);
	result.m[2][2] = ww - xx - yy + zz;

	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Matrix4x4::MakeTranslate(const Vector3& _v) {
	return Matrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		_v.x, _v.y, _v.z, 1.0f
	);
}

Matrix4x4 Matrix4x4::MakeAffine(const Vector3& _scale, const Vector3& _rotation, const Vector3& _translation) {
	Matrix4x4&& scale     = MakeScale(_scale);
	Matrix4x4&& rotate    = MakeRotate(_rotation);
	Matrix4x4&& translate = MakeTranslate(_translation);

	Matrix4x4&& result = scale * rotate * translate;

	return result;
}

Matrix4x4 Matrix4x4::MakeTranspose(const Matrix4x4& _matrix) {
	Matrix4x4 result{};
	for (size_t r = 0; r < 4; r++) {
		for (size_t c = 0; c < 4; c++) {
			result.m[r][c] = _matrix.m[c][r];
		}
	}
	return result;
}

Matrix4x4 Matrix4x4::MakeInverse(const Matrix4x4& _matrix) {
	XMVECTOR determinant;
	XMMATRIX inverseMatrix = XMMatrixInverse(&determinant, Convert(_matrix));

	return Convert(inverseMatrix);
}

Vector3 Matrix4x4::Transform(const Vector3& _v, const Matrix4x4& _m) {
	Vector3 result{};

	result.x = _v.x * _m.m[0][0] + _v.y * _m.m[1][0] + _v.z * _m.m[2][0] + 1.0f * _m.m[3][0];
	result.y = _v.x * _m.m[0][1] + _v.y * _m.m[1][1] + _v.z * _m.m[2][1] + 1.0f * _m.m[3][1];
	result.z = _v.x * _m.m[0][2] + _v.y * _m.m[1][2] + _v.z * _m.m[2][2] + 1.0f * _m.m[3][2];

	return result;
}

Vector4 Matrix4x4::Transform(const Vector4& _v, const Matrix4x4& _m) {
	Vector4 result{};

	result.x = _v.x * _m.m[0][0] + _v.y * _m.m[1][0] + _v.z * _m.m[2][0] + _v.w * _m.m[3][0];
	result.y = _v.x * _m.m[0][1] + _v.y * _m.m[1][1] + _v.z * _m.m[2][1] + _v.w * _m.m[3][1];
	result.z = _v.x * _m.m[0][2] + _v.y * _m.m[1][2] + _v.z * _m.m[2][2] + _v.w * _m.m[3][2];
	result.w = _v.x * _m.m[0][3] + _v.y * _m.m[1][3] + _v.z * _m.m[2][3] + _v.w * _m.m[3][3];

	return result;
}

Matrix4x4 Matrix4x4::Transpose() const {
	return MakeTranspose(*this);
}

Matrix4x4 Matrix4x4::Inverse() const {
	return MakeInverse(*this);
}

Vector3 Matrix4x4::ExtractScale() const {
	/// 3x3に変換
	float m3x3[3][3] = {
		{ m3x3[0][0], m3x3[0][1], m3x3[0][2] },
		{ m3x3[1][0], m3x3[1][1], m3x3[1][2] },
		{ m3x3[2][0], m3x3[2][1], m3x3[2][2] }
	};

	/// スケール成分を計算
	float scale[3];
	for (size_t i = 0; i < 3; i++) {
		scale[i] = std::sqrt(m3x3[i][0] * m3x3[i][0] + m3x3[i][1] * m3x3[i][1] + m3x3[i][2] * m3x3[i][2]);
	}

	return Vector3(scale[0], scale[1], scale[2]);
}

Quaternion Matrix4x4::ExtractRotation() const {
	Matrix4x4 matrix = *this;
	Vector3 scale = matrix.ExtractScale();
	/// スケールを除去
	matrix.m[0][0] /= scale.x;
	matrix.m[0][1] /= scale.x;
	matrix.m[0][2] /= scale.x;
	matrix.m[1][0] /= scale.y;
	matrix.m[1][1] /= scale.y;
	matrix.m[1][2] /= scale.y;
	matrix.m[2][0] /= scale.z;
	matrix.m[2][1] /= scale.z;
	matrix.m[2][2] /= scale.z;

	/// 回転成分を抽出
	return Quaternion::FromRotationMatrix(matrix);
}

Vector3 Matrix4x4::ExtractTranslation() const {
	return Vector3(
		m[3][0],
		m[3][1],
		m[3][2]
	);
}


