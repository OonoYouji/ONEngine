#include "Vector2.h"

#include <cmath>

//#include "Objects/Camera/Manager/CameraManager.h"
//#include "Objects/Camera/Manager/CameraManager.h"
//#include "WindowManager/WinApp.h"
//
//#include "Math/Vector3.h"
//#include "Math/Matrix4x4.h"

/// ===================================================
/// static objects initialize
/// ===================================================

const Vector2 Vector2::kUp = Vector2(0, 1);		//- ( 0,  1 )
const Vector2 Vector2::kDown = Vector2(0, -1);	//- ( 0, -1 )
const Vector2 Vector2::kLeft = Vector2(-1, 0);	//- (-1,  0 )
const Vector2 Vector2::kRight = Vector2(1, 0);	//- ( 1,  0 )
const Vector2 Vector2::kOne = Vector2(1, 1);	//- ( 1,  1 )


Vector2::Vector2() {
	this->x = 0.0f;
	this->y = 0.0f;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

float Vector2::Length() const {
	return std::sqrt(x * x + y * y);
}

Vector2 Vector2::Normalize() const {
	float len = this->Length();
	if(len != 0.0f) {
		return (*this) / len;
	}
	return *this;
}

float Vector2::Dot(const Vector2& v1, const Vector2& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y);
}

Vector2 Vector2::Lerp(const Vector2& v1, const Vector2& v2, float t) {
	return Vector2(
		std::lerp(v1.x, v2.x, t),
		std::lerp(v1.y, v2.y, t)
	);
}
