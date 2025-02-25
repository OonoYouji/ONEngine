#include "Vector3.h"

#include <cmath>
#include <algorithm>

#include "Vector2.h"
//#include "Objects/Camera/Manager/CameraManager.h"
//#include "WindowManager/WinApp.h"
//
//#include "Matrix4x4.h"


/// ===================================================
/// static objects initialize
/// ===================================================

const Vector3 Vector3::kUp    = Vector3( 0,  1,  0);	//- ( 0,  1,  0 )
const Vector3 Vector3::kDown  = Vector3( 0, -1,  0);	//- ( 0, -1,  0 )
const Vector3 Vector3::kLeft  = Vector3(-1,  0,  0);	//- (-1,  0,  0 )
const Vector3 Vector3::kRight = Vector3( 1,  0,  0);	//- ( 1,  0,  0 )
const Vector3 Vector3::kFront = Vector3( 0,  0,  1);	//- ( 0,  0,  1 )
const Vector3 Vector3::kBack  = Vector3( 0,  0, -1);	//- ( 0,  0, -1 )

const Vector3 Vector3::kOne   = Vector3(1, 1, 1);	//- ( 1,  1,  1 )
const Vector3 Vector3::kZero  = Vector3(0, 0, 0);	//- ( 1,  1,  1 )


Vector3::Vector3() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

Vector3::Vector3(float _x, float _y, float _z) {
	this->x = _x;
	this->y = _y;
	this->z = _z;
}

Vector3::Vector3(const Vector2& _v, float _z) {
	this->x = _v.x;
	this->y = _v.y;
	this->z = _z;
}



float Vector3::Len() const {
	return std::sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize() const {
	float len = this->Len();
	if(len != 0.0f) {
		return *this / len;
	}
	return *this;
}

float Vector3::Length(const Vector3& _v) {
	return _v.Len();
}

Vector3 Vector3::Normalize(const Vector3& _v) {
	return _v.Normalize();
}

Vector3 Vector3::Lerp(const Vector3& _start, const Vector3& _end, float _t) {
	return Vector3(
		std::lerp(_start.x, _end.x, _t),
		std::lerp(_start.y, _end.y, _t),
		std::lerp(_start.z, _end.z, _t)
	);
}

Vector3 Vector3::Slerp(const Vector3& _start, const Vector3& _end, float _t) {
	Vec3 nv1 = Normalize(_start);
	Vec3 nv2 = Normalize(_end);

	float dot = Dot(nv1, nv2);

	float theta = std::acos(dot);

	float sinTheta = std::sin(theta);
	float sinThetaFrom = std::sin((1.0f - _t) * theta);
	float sinThetaTo = std::sin(_t * theta);

	Vec3 nLerpVector = nv1 * (sinThetaFrom / sinTheta) + nv2 * (sinThetaTo / sinTheta);
	if(sinTheta < 1.0e-5) {
		nLerpVector = nv1;
	} else {
		nLerpVector = (nv1 * sinThetaFrom + nv2 * sinThetaTo) / sinTheta;
	}

	float length1 = Length(_start);
	float length2 = Length(_end);
	float length = std::lerp(length1, length2, _t);

	return nLerpVector * length;
}

Vector3 Vector3::Cross(const Vector3& _v1, const Vector3& _v2) {
	return Vector3(
		_v1.y * _v2.z - _v1.z * _v2.y,
		_v1.z * _v2.x - _v1.x * _v2.z,
		_v1.x * _v2.y - _v1.y * _v2.x
	);
}

float Vector3::Dot(const Vector3& _v1, const Vector3& _v2) {
	return (_v1.x * _v2.x) + (_v1.y * _v2.y) + (_v1.z * _v2.z);
}

Vector3 Vector3::MaxDotVector(const Vector3& _direction, const std::vector<Vector3>& _vertices) {
	float maxDot = Dot(_vertices.front(), _direction);
	Vec3 maxVertex = _vertices.front();
	for(auto& vertex : _vertices) {
		float dot = Dot(vertex, _direction);
		if(dot > maxDot) {
			maxDot = dot;
			maxVertex = vertex;
		}
	}

	return maxVertex;
}

Vector3 Vector3::MinDotVector(const Vector3& _direction, const std::vector<Vector3>& _vertices) {
	float minDot = Dot(_vertices.front(), _direction);
	Vec3 minVertex = _vertices.front();
	for(auto& vertex : _vertices) {
		float dot = Dot(vertex, _direction);
		if(dot < minDot) {
			minDot = dot;
			minVertex = vertex;
		}
	}

	return minVertex;
}
