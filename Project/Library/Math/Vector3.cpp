#include "Vector3.h"

#include <cmath>
#include <algorithm>

#include "Objects/Camera/Manager/CameraManager.h"
#include "WindowManager/WinApp.h"

#include "Vector2.h"
#include "Matrix4x4.h"


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

Vector3::Vector3(const Vector2& v, float z) {
	this->x = v.x;
	this->y = v.y;
	this->z = z;
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

float Vector3::Lenght(const Vector3& v) {
	return v.Len();
}

Vector3 Vector3::Normalize(const Vector3& v) {
	return v.Normalize();
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float t) {
	return Vector3(
		std::lerp(start.x, end.x, t),
		std::lerp(start.y, end.y, t),
		std::lerp(start.z, end.z, t)
	);
}

Vector3 Vector3::Slerp(const Vector3& start, const Vector3& end, float t) {
	Vec3 nv1 = Normalize(start);
	Vec3 nv2 = Normalize(end);

	float dot = Dot(nv1, nv2);

	float theta = std::acos(dot);

	float sinTheta = std::sin(theta);
	float sinThetaFrom = std::sin((1.0f - t) * theta);
	float sinThetaTo = std::sin(t * theta);

	Vec3 nLerpVector = nv1 * (sinThetaFrom / sinTheta) + nv2 * (sinThetaTo / sinTheta);
	if(sinTheta < 1.0e-5) {
		nLerpVector = nv1;
	} else {
		nLerpVector = (nv1 * sinThetaFrom + nv2 * sinThetaTo) / sinTheta;
	}

	float length1 = Lenght(start);
	float length2 = Lenght(end);
	float length = std::lerp(length1, length2, t);

	return nLerpVector * length;
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {
	return Vector3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

Vector3 Vector3::MaxDotVector(const Vector3& direction, const std::vector<Vector3>& vertices) {
	float maxDot = Dot(vertices.front(), direction);
	Vec3 maxVertex = vertices.front();
	for(auto& vertex : vertices) {
		float dot = Dot(vertex, direction);
		if(dot > maxDot) {
			maxDot = dot;
			maxVertex = vertex;
		}
	}

	return maxVertex;
}

Vector3 Vector3::MinDotVector(const Vector3& direction, const std::vector<Vector3>& vertices) {
	float minDot = Dot(vertices.front(), direction);
	Vec3 minVertex = vertices.front();
	for(auto& vertex : vertices) {
		float dot = Dot(vertex, direction);
		if(dot < minDot) {
			minDot = dot;
			minVertex = vertex;
		}
	}

	return minVertex;
}

Vector3 Vector3::ConvertWorld(const Vector2& screen, float distance) {
	Mat4 matViewport = Mat4::MakeViewport(0, 0, ONE::WinApp::kWindowSizeX, ONE::WinApp::kWindowSizeY, 0.0f, 1.0f);
	BaseCamera* camera = CameraManager::GetInstance()->GetMainCamera();
	Mat4 matInverseVPV = Mat4::MakeInverse(camera->GetMatView() * camera->GetMatProjection() * matViewport);

	Vec3 nearPosition = Mat4::Transform(Vec3(screen.x, screen.y, 0.0f), matInverseVPV);
	Vec3 farPosition = Mat4::Transform(Vec3(screen.x, screen.y, 1.0f), matInverseVPV);
	Vec3 mouseDirection = Normalize(farPosition - nearPosition);

	return nearPosition + (mouseDirection * distance);
}
