#include "Camera.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/Utility/Input/Input.h"

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

	if (Input::PressKey(DIK_W)) { transform_->position.z += 0.1f; }
	if (Input::PressKey(DIK_S)) { transform_->position.z -= 0.1f; }
	if (Input::PressKey(DIK_A)) { transform_->position.x -= 0.1f; }
	if (Input::PressKey(DIK_D)) { transform_->position.x += 0.1f; }
	if (Input::PressKey(DIK_SPACE)) { transform_->position.y += 0.1f; }
	if (Input::PressKey(DIK_LSHIFT)) { transform_->position.y -= 0.1f; }

	const float speed = std::numbers::pi_v<float> / 100.0f;
	if (Input::PressKey(DIK_UP)) { transform_->rotate.x -= speed; }
	if (Input::PressKey(DIK_DOWN)) { transform_->rotate.x += speed; }
	if (Input::PressKey(DIK_LEFT)) { transform_->rotate.y -= speed; }
	if (Input::PressKey(DIK_RIGHT)) { transform_->rotate.y += speed; }



	transform_->Update();

	matView_       = MakeViewMatrix(transform_->GetMatWorld());
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, 1280.0f / 720.0f,
		nearClip_, farClip_
	);

	viewProjection_->SetMappingData(ViewProjection(matView_ * matProjection_));

}

float Camera::Cot(float _t) const {
	return 1.0f / std::tanf(_t);
}

Matrix4x4 Camera::MakeViewMatrix(const Matrix4x4& _matWorld) const {
	return Matrix4x4::MakeInverse(_matWorld);
}

Matrix4x4 Camera::MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip) const {
	return Matrix4x4(
		(1 / _aspectRatio) * Cot(_fovY / 2.0f), 0.0f,              0.0f,                                             0.0f,
		0.0f,                                   Cot(_fovY / 2.0f), 0.0f,                                             0.0f,
		0.0f,                                   0.0f,              _farClip / (_farClip - _nearClip),                1.0f,
		0.0f,                                   0.0f,              (-_nearClip * _farClip) / (_farClip - _nearClip), 0.0f
	);
}

