#define NOMINMAX
#include <BaseCamera.h>

#include <WinApp.h>
#include <DxResourceCreator.h>

#include <ImGuiManager.h>
#include <WorldTime.h>
#include <Easing.h>

#include "Shake/Shake.h"

namespace {

	float Cot(float t) {
		return 1.0f / std::tanf(t);
	}

	Mat4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
		return Mat4(
			{ (1 / aspectRatio) * Cot(fovY / 2.0f), 0.0f, 0.0f, 0.0f },
			{ 0.0f, Cot(fovY / 2.0f), 0.0f, 0.0f },
			{ 0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f },
			{ 0.0f, 0.0f, (-nearClip * farClip) / (farClip - nearClip), 0.0f }
		);
	}

}


/// ===================================================
/// コンストラクタ
/// ===================================================
BaseCamera::BaseCamera() {
	CreateTag(this);
}



/// ===================================================
/// 初期化
/// ===================================================
void BaseCamera::BaseInitialize() {

	transform_.Initialize();
	UpdateMatView();
	UpdateMatProjection();

	matVp_ = matView_ * matProjection_;

	viewProjectionBuffer_ = ONE::DxResourceCreator::CreateResource(sizeof(Mat4));
	viewProjectionBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matVpData_));
	*matVpData_ = matVp_;

	Transfer();

	shake_ = new Shake();
	shake_->Initialize();
	SetParent(shake_);
}

void BaseCamera::BaseUpdate() {
	UpdateMatrix();
	UpdateMatView();
	UpdateMatProjection();
	Transfer();
}

void BaseCamera::UpdateMatView() {
	matView_ = transform_.matTransform.Inverse();
}

void BaseCamera::UpdateMatProjection() {
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, static_cast<float>(ONE::WinApp::kWindowSizeX) / static_cast<float>(ONE::WinApp::kWindowSizeY),
		0.1f, farZ_);
}

void BaseCamera::Move() {
	if(moveTime_ > maxMoveTime_) { return; }

	moveTime_ += WorldTime::DeltaTime();

	float t = Ease::Out::Quart(std::min(moveTime_ / maxMoveTime_, 1.0f));
	Vec3 position = Vec3::Lerp(startMoveData_.position, endMoveData_.position, t);
	Vec3 rotate = Vec3::Lerp(startMoveData_.rotate, endMoveData_.rotate, t);
	SetPosition(position);
	SetRotate(rotate);
}



void BaseCamera::Transfer() {
	matVp_ = matView_ * matProjection_;
	*matVpData_ = matVp_;
}

void BaseCamera::SetMove(const MoveData& start, const MoveData& end, float time) {
	moveTime_ = 0.0f;
	maxMoveTime_ = time;

	startMoveData_ = start;
	endMoveData_ = end;
}

