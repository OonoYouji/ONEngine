#include "Camera.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/Utility/Input/Input.h"



namespace {

	float Cot(float _t) {
		return 1.0f / std::tan(_t);
	}

} /// unnamed namespace



Camera::Camera(DxDevice* _dxDevice) {

	viewProjection_ = std::make_unique<ConstantBuffer<ViewProjection>>();
	viewProjection_->Create(_dxDevice);
	viewProjection_->SetMappingData(ViewProjection(
		Matrix4x4::kIdentity
	));
}

Camera::~Camera() {}


void Camera::Initialize() {

	transform_->position = { 0.0f, 0.0f, -10.0f };
	transform_->scale    = Vector3::kOne;
	transform_->rotate   = Vector3::kZero;

	fovY_     = 0.7f;
	nearClip_ = 0.1f;
	farClip_  = 1000.0f;
}

void Camera::Update() {

	Vector3 velocity = Vector3::kZero;

	if (Input::PressKey(DIK_W)) { velocity.z += 0.1f; }
	if (Input::PressKey(DIK_S)) { velocity.z -= 0.1f; }
	if (Input::PressKey(DIK_A)) { velocity.x -= 0.1f; }
	if (Input::PressKey(DIK_D)) { velocity.x += 0.1f; }

	velocity = Matrix4x4::Transform(velocity, Matrix4x4::MakeRotateY(transform_->rotate.y));

	if (Input::PressKey(DIK_SPACE)) { velocity.y += 0.1f; }
	if (Input::PressKey(DIK_LSHIFT)) { velocity.y -= 0.1f; }


	const float speed = std::numbers::pi_v<float> / 100.0f;
	if (Input::PressKey(DIK_UP)) { transform_->rotate.x -= speed; }
	if (Input::PressKey(DIK_DOWN)) { transform_->rotate.x += speed; }
	if (Input::PressKey(DIK_LEFT)) { transform_->rotate.y -= speed; }
	if (Input::PressKey(DIK_RIGHT)) { transform_->rotate.y += speed; }

	transform_->position += velocity;
	transform_->Update();

	matView_       = transform_->GetMatWorld().Inverse();
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, 1280.0f / 720.0f,
		nearClip_, farClip_
	);

	viewProjection_->SetMappingData(ViewProjection(matView_ * matProjection_));

}



Matrix4x4 Camera::MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip) const {
	return Matrix4x4(
		(1 / _aspectRatio) * Cot(_fovY / 2.0f), 0.0f,              0.0f,                                             0.0f,
		0.0f,                                   Cot(_fovY / 2.0f), 0.0f,                                             0.0f,
		0.0f,                                   0.0f,              _farClip / (_farClip - _nearClip),                1.0f,
		0.0f,                                   0.0f,              (-_nearClip * _farClip) / (_farClip - _nearClip), 0.0f
	);
}



/// ===================================================
/// 2Dカメラ
/// ===================================================

Camera2D::Camera2D(DxDevice* _dxDevice) {
	viewProjection_ = std::make_unique<ConstantBuffer<ViewProjection>>();
	viewProjection_->Create(_dxDevice);
	viewProjection_->SetMappingData(ViewProjection(
		Matrix4x4::kIdentity
	));
}

Camera2D::~Camera2D() {}

void Camera2D::Initialize() {

	transform_->position = { 0.0f, 0.0f, -10.0f };
	transform_->scale    = Vector3::kOne;
	transform_->rotate   = Vector3::kZero;

	matProjection_ = MakeOrthographicMatrix(
		-640.0f, 640.0f, -360.0f, 360.0f, 0.1f, 1.0f
	);
}

void Camera2D::Update() {


	//{	/// カメラ移動
	//	Vector3 velocity = Vector3::kZero;

	//	if (Input::PressKey(DIK_W)) { velocity.y += 0.1f; }
	//	if (Input::PressKey(DIK_S)) { velocity.y -= 0.1f; }
	//	if (Input::PressKey(DIK_A)) { velocity.x -= 0.1f; }
	//	if (Input::PressKey(DIK_D)) { velocity.x += 0.1f; }

	//	velocity = velocity.Normalize() * 10.0f;
	//	transform_->position += velocity;
	//}

	transform_->Update();

	Matrix4x4 matScale = Matrix4x4::MakeScale(transform_->scale);
	if (transform_->GetParent()) {
		matScale *= Matrix4x4::MakeScale(transform_->GetParent()->scale);
	}

	matView_       = (transform_->GetMatWorld() * matScale.Inverse()).Inverse();
	viewProjection_->SetMappingData(ViewProjection(matView_ * matProjection_));

}

Matrix4x4 Camera2D::MakeOrthographicMatrix(float _left, float _right, float _bottom, float _top, float _znear, float _zfar) const {
	Matrix4x4 result = {};

	float width  = _right - _left;
	float height = _top - _bottom;
	float depth  = _zfar - _znear;

	result.m[0][0] = 2.0f / width;
	result.m[1][1] = 2.0f / height;
	result.m[2][2] = 1.0f / depth;
	result.m[3][0] = -(_right + _left) / width;
	result.m[3][1] = -(_top + _bottom) / height;
	result.m[3][2] = -_znear / depth;
	result.m[3][3] = 1.0f;

	return result;
}
