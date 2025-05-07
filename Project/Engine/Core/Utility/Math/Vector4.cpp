#include "Vector4.h"

#include <cmath>

//#include "Vector3.h"

/// ===================================================
/// static objects initialize
/// ===================================================

const Vector4 Vector4::k1000 = { 1,0,0,0 };
const Vector4 Vector4::k0100 = { 0,1,0,0 };
const Vector4 Vector4::k0010 = { 0,0,1,0 };
const Vector4 Vector4::k0001 = { 0,0,0,1 };

const Vector4 Vector4::kWhite = { 1,1,1,1 };
const Vector4 Vector4::kRed   = { 1,0,0,1 };
const Vector4 Vector4::kGreen = { 0,1,0,1 };
const Vector4 Vector4::kBlue  = { 0,0,1,1 };

const Vector4 Vector4::kZero = { 0,0,0,0 };


Vector4::Vector4() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 0.0f;
}

Vector4::Vector4(float _x, float _y, float _z, float _w) {
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->w = _w;
}

Vector4::Vector4(const Vector3& _v, float _w) {
	this->x = _v.x;
	this->y = _v.y;
	this->z = _v.z;
	this->w = _w;
}


Vector4 Vector4::Lerp(const Vector4& _v1, const Vector4& _v2, float _t) {
	return Vector4(
		std::lerp(_v1.x, _v2.x, _t),
		std::lerp(_v1.y, _v2.y, _t),
		std::lerp(_v1.z, _v2.z, _t),
		std::lerp(_v1.w, _v2.w, _t)
	);
}

float Vector4::Length(const Vector4& _v) {
	return std::sqrt(
		_v.x * _v.x +
		_v.y * _v.y +
		_v.z * _v.z +
		_v.w * _v.w
	);
}

Vector4 Vector4::Normalize(const Vector4& _v) {
	float len = Length(_v);
	if(len != 0.0f) {
		return _v / len;
	}
	return _v;
}

Vector4 Vector4::Convert(const Vector3& _v, float _w) {
	return Vector4(_v, _w);
}

Vector3 Vector4::Convert(const Vector4& _v) {
	return Vector3(
		_v.x / _v.w,
		_v.y / _v.w,
		_v.z / _v.w
	);
}
