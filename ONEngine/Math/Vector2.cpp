#include <Vector2.h>

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
