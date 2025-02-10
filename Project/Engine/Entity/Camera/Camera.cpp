#include "Camera.h"

Camera::Camera(DxDevice* _dxDevice) {

	viewProjection_ = std::make_unique<ConstantBuffer<ViewProjection>>();
	viewProjection_->Create(_dxDevice);
	viewProjection_->SetMappingData(ViewProjection(
		Matrix4x4::kIdentity,
		Matrix4x4::kIdentity,
		Matrix4x4::kIdentity
	));
}

Camera::~Camera() {}


void Camera::Initialize() {

	transform_->position = { 0.0f, 0.0f, -10.0f };

}

void Camera::Update() {

	transform_->Update();
	Matrix4x4&& matView       = MakeViewMatrix(transform_->GetMatWorld());
	Matrix4x4&& matProjection = MakePerspectiveFovMatrix(
		fovY_, 1280.0f / 720.0f,
		nearClip_, farClip_
	);

	viewProjection_->SetMappingData(ViewProjection(
		matView,
		matProjection,
		matView * matProjection
	));

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

