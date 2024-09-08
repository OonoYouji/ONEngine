#include <BaseCamera.h>

#include <WinApp.h>
#include <DxResourceCreator.h>

#include <ImGuiManager.h>
#include <WorldTime.h>

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

void BaseCamera::UpdateMatView() {
	matView_ = transform_.matTransform.Inverse();
}

void BaseCamera::UpdateMatProjection() {
	matProjection_ = MakePerspectiveFovMatrix(
		fovY_, static_cast<float>(ONE::WinApp::kWindowSizeX) / static_cast<float>(ONE::WinApp::kWindowSizeY),
		0.1f, farZ_);
}

void BaseCamera::Transfer() {
	matVp_ = matView_ * matProjection_;
	*matVpData_ = matVp_;
}




void Shake::Initialize() {
	isDrawActive = false;
	time_ = 0.0f;
	speed_ = 1.0f;
	amplitude_ = 1.0f;
}

void Shake::Update() {
	if(time_ <= 0.0f) {
		transform_.position = { 0,0,0 };
		return;
	}
	time_ -= WorldTime::DeltaTime();
	auto lRandom = []() -> float {return (rand() % 20 - 9) / 10.0f; };
	Vec3 randomV = { lRandom(), lRandom(),lRandom() };
	transform_.position = randomV * (amplitude_ * speed_);
	transform_.position.z = 0.0f;
}

void Shake::Debug() {
	if(ImGui::TreeNodeEx("shake", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("time", &time_, 0.01f);
		ImGui::DragFloat("speed", &speed_, 0.01f);
		ImGui::DragFloat("amplitude", &amplitude_, 0.01f);
		ImGui::TreePop();
	}
}

void Shake::Setting(float time, float speed, float amplitude) {
	time_ = time;
	speed_ = speed;
	amplitude_ = amplitude;
}
