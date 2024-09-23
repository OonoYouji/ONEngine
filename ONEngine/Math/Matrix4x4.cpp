#include <Matrix4x4.h>

#include <Vector3.h>
#include <Vector4.h>


namespace {

	inline XMMATRIX ComputeInverseMatrix(const XMMATRIX& matrix) {
		XMVECTOR determinant;
		XMMATRIX inverseMatrix = XMMatrixInverse(&determinant, matrix);
		return inverseMatrix;
	}

	inline XMMATRIX CreateOrthographicMatrix(float width, float height, float nearZ, float farZ) {
		return XMMatrixOrthographicOffCenterLH(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, nearZ, farZ);
	}

}


/// ===================================================
/// static objects initialize
/// ===================================================

const Matrix4x4 Matrix4x4::kIdentity = Matrix4x4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { 0,0,0,1 });



Matrix4x4::Matrix4x4() {
	(*this) = kIdentity;
}

Matrix4x4::Matrix4x4(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4) {
	this->m[0][0] = row1.x;
	this->m[0][1] = row1.y;
	this->m[0][2] = row1.z;
	this->m[0][3] = row1.w;

	this->m[1][0] = row2.x;
	this->m[1][1] = row2.y;
	this->m[1][2] = row2.z;
	this->m[1][3] = row2.w;

	this->m[2][0] = row3.x;
	this->m[2][1] = row3.y;
	this->m[2][2] = row3.z;
	this->m[2][3] = row3.w;

	this->m[3][0] = row4.x;
	this->m[3][1] = row4.y;
	this->m[3][2] = row4.z;
	this->m[3][3] = row4.w;

}

Matrix4x4::Matrix4x4(float aa, float ab, float ac, float ad, float ba, float bb, float bc, float bd, float ca, float cb, float cc, float cd, float da, float db, float dc, float dd) {
	this->m[0][0] = aa;
	this->m[0][1] = ab;
	this->m[0][2] = ac;
	this->m[0][3] = ad;

	this->m[1][0] = ba;
	this->m[1][1] = bb;
	this->m[1][2] = bc;
	this->m[1][3] = bd;

	this->m[2][0] = ca;
	this->m[2][1] = cb;
	this->m[2][2] = cc;
	this->m[2][3] = cd;

	this->m[3][0] = da;
	this->m[3][1] = db;
	this->m[3][2] = dc;
	this->m[3][3] = dd;
}

Matrix4x4::Matrix4x4(const XMMATRIX& xm) {
	(*this) = ConvertToMatrix4x4(xm);
}

Matrix4x4 Matrix4x4::Inverse() const {
	return Matrix4x4(ComputeInverseMatrix(ConvertToXMMATRIX((*this))));
}



/// ===================================================
/// static methods
/// ===================================================

Matrix4x4 Matrix4x4::MakeScale(const Vector3& v) {
	return Matrix4x4(
		{ v.x, 0.0f, 0.0f, 0.0f },
		{ 0.0f,  v.y, 0.0f, 0.0f },
		{ 0.0f, 0.0f,  v.z, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	);
}

Matrix4x4 Matrix4x4::MakeRotateX(float theta) {
	return Matrix4x4(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f,  std::cos(theta), std::sin(theta), 0.0f },
		{ 0.0f, -std::sin(theta), std::cos(theta), 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	);
}

Matrix4x4 Matrix4x4::MakeRotateY(float theta) {
	return Matrix4x4(
		{ std::cos(theta), 0.0f, -std::sin(theta), 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ std::sin(theta), 0.0f,  std::cos(theta), 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	);
}

Matrix4x4 Matrix4x4::MakeRotateZ(float theta) {
	return Matrix4x4(
		{ std::cos(theta), std::sin(theta), 0.0f, 0.0f },
		{ -std::sin(theta), std::cos(theta), 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	);
}

Matrix4x4 Matrix4x4::MakeRotate(const Vector3& v) {
	return MakeRotateX(v.x) * MakeRotateY(v.y) * MakeRotateZ(v.z);
}

Matrix4x4 Matrix4x4::MakeTranslate(const Vector3& v) {
	return Matrix4x4(
		{ 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ v.x,  v.y,  v.z, 1.0f }
	);
}

Matrix4x4 Matrix4x4::MakeAffine(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	return MakeScale(scale) * MakeRotate(rotate) * MakeTranslate(translate);
}

Matrix4x4 Matrix4x4::MakeInverse(const Matrix4x4& m) {
	return m.Inverse();
}

Matrix4x4 Matrix4x4::MakeViewport(float top, float left, float width, float height, float minD, float maxD) {
	return Matrix4x4{
		width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -(height / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, maxD - minD, 0.0f,
		left + (width / 2.0f), top + (height / 2.0f), minD, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float l, float t, float r, float b, float nearZ, float farZ) {
	return Mat4(
		{ 2.0f / (r - l),    0.0f,              0.0f,                             0.0f },
		{ 0.0f,              2.0f / (t - b),    0.0f,                             0.0f },
		{ 0.0f,              0.0f,              -2.0f / (farZ - nearZ),           0.0f },
		{ -(l + r) / (r - l), -(t + b) / (t - b), -(farZ + nearZ) / (farZ - nearZ), 1.0f }
	);


}

Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float width, float height, float nearZ, float farZ) {
	return ConvertToMatrix4x4(CreateOrthographicMatrix(width, height, nearZ, farZ));
}

Vector3 Matrix4x4::Transform(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{};

	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2];
	float w  = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];

	assert(w != 0.0f);
	return result / w;
}

Vector3 Matrix4x4::TransformNormal(const Vector3& v, const Matrix4x4& m) {
	return Vector3{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};
}

