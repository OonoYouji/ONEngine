#include "Vector4.h"

#include <cmath>


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


Vector4::Vector4() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 0.0f;
}

Vector4::Vector4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}


Vector4 Vector4::Lerp(const Vector4& start, const Vector4& end, float t) {
	return Vector4(
		std::lerp(start.x, end.x, t),
		std::lerp(start.y, end.y, t),
		std::lerp(start.z, end.z, t),
		std::lerp(start.w, end.w, t)
	);
}

float Vector4::Lenght(const Vector4& v) {
	return std::sqrt(
		v.x * v.x +
		v.y * v.y +
		v.z * v.z +
		v.w * v.w
	);
}

Vector4 Vector4::Normalize(const Vector4& v) {
	float len = Lenght(v);
	if(len != 0.0f) {
		return v / len;
	}
	return v;
}
