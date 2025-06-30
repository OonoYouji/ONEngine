#include "Camera.h"

/// std
#include <numbers>

/// engine
#include "Engine/Core/Utility/Input/Input.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


namespace {

	float Cot(float _t) {
		return 1.0f / std::tan(_t);
	}

} /// unnamed namespace



Camera::Camera(DxDevice* _dxDevice) {
	SetCameraType(static_cast<int>(CameraType::Type3D));
	viewProjection_ = std::make_unique<ConstantBuffer<ViewProjection>>();
	viewProjection_->Create(_dxDevice);
	viewProjection_->SetMappedData(ViewProjection(
		Matrix4x4::kIdentity
	));
}

Camera::~Camera() {
	if (pEntityComponentSystem_->GetMainCamera() == this) {
		pEntityComponentSystem_->SetMainCamera(nullptr);
	}

	if (pEntityComponentSystem_->GetMainCamera2D() == this) {
		pEntityComponentSystem_->SetMainCamera2D(nullptr);
	}

	if (pEntityComponentSystem_->GetDebugCamera() == this) {
		pEntityComponentSystem_->SetDebugCamera(nullptr);
	}
}


void Camera::Initialize() {
	fovY_ = 0.7f;
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;
}

void Camera::Update() {
	//static bool isActive = true;
	//if (Input::TriggerKey(DIK_TAB)) {
	//	isActive = !isActive;
	//}

	//if (!isActive) { return; }

	//Vector3 velocity = Vector3::kZero;

	//if (Input::PressKey(DIK_W)) { velocity.z += 0.1f; }
	//if (Input::PressKey(DIK_S)) { velocity.z -= 0.1f; }
	//if (Input::PressKey(DIK_A)) { velocity.x -= 0.1f; }
	//if (Input::PressKey(DIK_D)) { velocity.x += 0.1f; }

	//velocity = Matrix4x4::Transform(velocity, Matrix4x4::MakeRotateY(transform_->rotate.y));

	//if (Input::PressKey(DIK_SPACE)) { velocity.y += 0.1f; }
	//if (Input::PressKey(DIK_LSHIFT)) { velocity.y -= 0.1f; }


	//const float speed = std::numbers::pi_v<float> / 100.0f;
	//if (Input::PressKey(DIK_UP)) { transform_->rotate.x -= speed; }
	//if (Input::PressKey(DIK_DOWN)) { transform_->rotate.x += speed; }
	//if (Input::PressKey(DIK_LEFT)) { transform_->rotate.y -= speed; }
	//if (Input::PressKey(DIK_RIGHT)) { transform_->rotate.y += speed; }

	//transform_->position += velocity * 10.0f;


	//transform_->Update();
	UpdateTransform();
	matView_ = transform_->GetMatWorld().Inverse();

	if (cameraType_ == static_cast<int>(CameraType::Type3D)) { ///!< 3Dカメラの場合
		matProjection_ = MakePerspectiveFovMatrix(
			fovY_, 1920.0f / 1080.0f,
			nearClip_, farClip_
		);
	}

	viewProjection_->SetMappedData(ViewProjection(matView_ * matProjection_));

}



Matrix4x4 Camera::MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip) const {
	return Matrix4x4(
		(1 / _aspectRatio) * Cot(_fovY / 2.0f), 0.0f, 0.0f, 0.0f,
		0.0f, Cot(_fovY / 2.0f), 0.0f, 0.0f,
		0.0f, 0.0f, _farClip / (_farClip - _nearClip), 1.0f,
		0.0f, 0.0f, (-_nearClip * _farClip) / (_farClip - _nearClip), 0.0f
	);
}

Matrix4x4 Camera::MakeOrthographicMatrix(float _left, float _right, float _bottom, float _top, float _znear, float _zfar) const {
	Matrix4x4 result = {};

	float width = _right - _left;
	float height = _top - _bottom;
	float depth = _zfar - _znear;

	result.m[0][0] = 2.0f / width;
	result.m[1][1] = 2.0f / height;
	result.m[2][2] = 1.0f / depth;
	result.m[3][0] = -(_right + _left) / width;
	result.m[3][1] = -(_top + _bottom) / height;
	result.m[3][2] = -_znear / depth;
	result.m[3][3] = 1.0f;

	return result;
}

void Camera::SetCameraType(int _cameraType) {
	if (_cameraType != cameraType_) {

		float widthHalf = 1920.0f / 2.0f;
		float heightHalf = 1080.0f / 2.0f;

		if (_cameraType == static_cast<int>(CameraType::Type2D)) {
			matProjection_ = MakeOrthographicMatrix(
				-widthHalf, widthHalf, -heightHalf, heightHalf, 0.1f, 1.0f
			);
		}

	}
	cameraType_ = _cameraType;

}

