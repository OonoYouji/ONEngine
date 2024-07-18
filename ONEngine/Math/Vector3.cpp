#include <Vector3.h>


/// ===================================================
/// static objects initialize
/// ===================================================

const Vector3 Vector3::kUp = Vector3(0, 1, 0);		//- ( 0,  1,  0 )
const Vector3 Vector3::kDown = Vector3(0, -1, 0);	//- ( 0, -1,  0 )
const Vector3 Vector3::kLeft = Vector3(-1, 0, 0);	//- (-1,  0,  0 )
const Vector3 Vector3::kRight = Vector3(1, 0, 0);	//- ( 1,  0,  0 )
const Vector3 Vector3::kFront = Vector3(0, 0, -1);	//- ( 0,  0, -1 )
const Vector3 Vector3::kBack = Vector3(0, 0, 1);	//- ( 0,  0,  1 )


Vector3::Vector3() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
