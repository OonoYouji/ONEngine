#include "Camera.h"

Camera::Camera(DxDevice* _dxDevice) {

	viewProjection_ = std::make_unique<ConstantBuffer<ViewProjection>>();
	viewProjection_->Create(_dxDevice);

}

Camera::~Camera() {}


void Camera::Initialize() {

}

void Camera::Update() {

}
