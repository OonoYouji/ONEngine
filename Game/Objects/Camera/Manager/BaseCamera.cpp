#include <BaseCamera.h>

#include <WinApp.h>
#include <CameraManager.h>

#include <Vector4.h>

#include <DxResourceCreator.h>

namespace {

	float Cot(float t) {
		return 1.0f / std::tanf(t);
	}

	Mat4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		return Mat4(
			{ (1 / aspectRatio) * Cot(fovY / 2.0f), 0.0f, 0.0f, 0.0f},
			{ 0.0f, Cot(fovY / 2.0f), 0.0f, 0.0f},
			{ 0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f},
			{ 0.0f, 0.0f, (-nearClip * farClip) / (farClip - nearClip), 0.0f}
		);
	}

}


/// ===================================================
/// コンストラクタ
/// ===================================================
BaseCamera::BaseCamera() : BaseGameObject(this) {

}



/// ===================================================
/// 初期化
/// ===================================================
void BaseCamera::BaseInitialize() {

	transform_.Initialize();
	matView_ = transform_.matTransform.Inverse();
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, static_cast<float>(ONE::WinApp::kWindowSizeX) / static_cast<float>(ONE::WinApp::kWindowSizeY),
		0.1f, farZ_);

	matVp_ = matView_ * matProjection_;

	viewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matVpData_));
	*matVpData_ = matVp_;

}

void BaseCamera::Transfer() {
	matVp_ = matView_ * matProjection_;
	*matVpData_ = matVp_;
}
