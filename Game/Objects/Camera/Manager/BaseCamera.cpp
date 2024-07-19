#include <BaseCamera.h>

#include <WinApp.h>
#include <CameraManager.h>

#include <Vector4.h>

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
BaseCamera::BaseCamera() {

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
}
