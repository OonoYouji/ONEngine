#include <Vector2.h>

#include <CameraManager.h>
#include <WinApp.h>

#include <Vector3.h>
#include <Matrix4x4.h>

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

float Vector2::Lenght() const {
	return std::sqrt(x * x + y + y);
}

Vector2 Vector2::Normalize() const {
	float len = this->Lenght();
	if(len != 0.0f) {
		return (*this) / len;
	}
	return *this;
}


Vector2 Vector2::ConvertScreen(const Vector3& world) {
	/// view projection viewport 行列の計算
	Mat4 matViewport = Mat4::MakeViewport(0, 0, ONE::WinApp::kWindowSizeX, ONE::WinApp::kWindowSizeY, 0.0f, 1.0f);
	BaseCamera* camera = CameraManager::GetInstance()->GetMainCamera();
	Matrix4x4 matVPV = camera->GetMatView() * camera->GetMatProjection() * matViewport;

	/// world -> screen計算
	Vec3 positionRaticle = Mat4::Transform(world, matVPV);
	return Vec2(positionRaticle.x, positionRaticle.y);
}

float Vector2::Dot(const Vector2& v1, const Vector2& v2) {
	return (v1.x * v2.x) + (v1.y * v2.y);
}
