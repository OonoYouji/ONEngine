#include <Vector4.h>


/// ===================================================
/// static objects initialize
/// ===================================================

const Vector4 Vector4::k1000 = { 1,0,0,0 };
const Vector4 Vector4::k0100 = { 0,1,0,0 };
const Vector4 Vector4::k0010 = { 0,0,1,0 };
const Vector4 Vector4::k0001 = { 0,0,0,1 };


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
