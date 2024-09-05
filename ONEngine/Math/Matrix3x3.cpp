#include <Matrix3x3.h>

#include <cmath>

#include <Vector2.h>
#include <Vector3.h>
#include <Matrix4x4.h>


const Matrix3x3 Matrix3x3::kIdentity = Matrix3x3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1));


Matrix3x3::Matrix3x3() {
	*this = kIdentity;
}

Matrix3x3::Matrix3x3(float aa, float ab, float ac, float ba, float bb, float bc, float ca, float cb, float cc) {
	this->m[0][0] = aa;
	this->m[0][1] = ab;
	this->m[0][2] = ac;

	this->m[1][0] = ba;
	this->m[1][1] = bb;
	this->m[1][2] = bc;

	this->m[2][0] = ca;
	this->m[2][1] = cb;
	this->m[2][2] = cc;
}

Matrix3x3::Matrix3x3(const Vector3& row1, const Vector3& row2, const Vector3& row3) {
	this->m[0][0] = row1.x;
	this->m[0][1] = row1.y;
	this->m[0][2] = row1.z;

	this->m[1][0] = row2.x;
	this->m[1][1] = row2.y;
	this->m[1][2] = row2.z;

	this->m[2][0] = row3.x;
	this->m[2][1] = row3.y;
	this->m[2][2] = row3.z;
}

Matrix4x4 Matrix3x3::ToMat4() const {
	return ToMat4(*this);
}

Matrix3x3 Matrix3x3::MakeScale(const Vector2& scale) {
	return Matrix3x3(
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f
	);
}

Matrix3x3 Matrix3x3::MakeRotate(float rotate) {
	return Matrix3x3(
		std::cos(rotate), std::sin(rotate), 0.0f,
		-std::sin(rotate), std::cos(rotate), 0.0f,
		0.0f, 0.0f, 1.0f
	);
}

Matrix3x3 Matrix3x3::MakeTranslate(const Vector2& translate) {
	return Matrix3x3(
		1, 0, 0,
		0, 1, 0,
		translate.x, translate.y, 1
	);
}

Matrix3x3 Matrix3x3::MakeAffine(const Vector2& scale, float rotate, const Vector2& translate) {
	Matrix3x3 S = MakeScale(scale);
	Matrix3x3 R = MakeRotate(rotate);
	Matrix3x3 T = MakeTranslate(translate);
	return S * R * T;
}

Matrix4x4 Matrix3x3::ToMat4(const Matrix3x3& m) {
	return Matrix4x4(
		m.m[0][0],m.m[0][1],m.m[0][2],0.0f,
		m.m[1][0],m.m[1][1],m.m[1][2],0.0f,
		m.m[2][0],m.m[2][1],m.m[2][2],0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}
