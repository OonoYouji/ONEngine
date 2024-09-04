#include <Mouse.h>

#include <cassert>
#include <cmath>

#include <WinApp.h>
#include <CameraManager.h>


Mouse::Mouse() {}
Mouse::~Mouse() {}



void Mouse::Initialize(IDirectInput8* directInput, ONE::WinApp* winApp) {

	pWinApp_ = winApp;

	HRESULT hr = directInput->CreateDevice(
		GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));

	hr = mouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(hr));

	///- 排他制御レベルのセット
	// DISCL_FOREGROUND   : 画面が手前にある場合のみ入力を受け付ける
	// DISCL_NONEXCLUSIVE : デバイスをこのアプリだけで占有しない
	// DISCL_NOWINKEY     : Windowsキーを無効にする

	hr = mouse_->SetCooperativeLevel(
		pWinApp_->GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}



void Mouse::Begin() {

	preState_ = state_;

	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(state_), &state_);
	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(pWinApp_->GetHWND(), &mousePos);

	position_ = Vec2(
		static_cast<float>(mousePos.x),
		static_cast<float>(mousePos.y)
	);

	velocity_ = Vec2(
		static_cast<float>(state_.lX),
		static_cast<float>(state_.lY)
	);
}

bool Mouse::Press(MouseCode code) const {
	return state_.rgbButtons[static_cast<uint32_t>(code)];
}

bool Mouse::Trigger(MouseCode code) const {
	return Press(code) && !preState_.rgbButtons[static_cast<uint32_t>(code)];
}

bool Mouse::Release(MouseCode code) const {
	return !Press(code) && preState_.rgbButtons[static_cast<uint32_t>(code)];
}

Vec3 Mouse::MouseRay(float distance) {
	Mat4 matViewport = Mat4::MakeViewport(0, 0, ONE::WinApp::kWindowSizeX, ONE::WinApp::kWindowSizeY, 0.0f, 1.0f);
	BaseCamera* camera = CameraManager::GetInstance()->GetMainCamera();
	Mat4 matInverseVPV = Mat4::MakeInverse(camera->GetMatView() * camera->GetMatProjection() * matViewport);

	Vec3 nearPos = Mat4::Transform(Vec3(position_.x, position_.y, 0.0f), matInverseVPV);
	Vec3 farPos = Mat4::Transform(Vec3(position_.x, position_.y, 1.0f), matInverseVPV);
	Vec3 direction = Vec3::Normalize(nearPos - farPos);

	return nearPos + (direction * distance);
}

Vec3 Mouse::MouseNearPosition() {
	Mat4 matViewport = Mat4::MakeViewport(0, 0, ONE::WinApp::kWindowSizeX, ONE::WinApp::kWindowSizeY, 0.0f, 1.0f);
	BaseCamera* camera = CameraManager::GetInstance()->GetMainCamera();
	Mat4 matInverseVPV = Mat4::MakeInverse(camera->GetMatView() * camera->GetMatProjection() * matViewport);

	return Mat4::Transform(Vec3(position_.x, position_.y, 0.0f), matInverseVPV);
}

Vec3 Mouse::MouseFarPosition() {
	Mat4 matViewport = Mat4::MakeViewport(0, 0, ONE::WinApp::kWindowSizeX, ONE::WinApp::kWindowSizeY, 0.0f, 1.0f);
	BaseCamera* camera = CameraManager::GetInstance()->GetMainCamera();
	Mat4 matInverseVPV = Mat4::MakeInverse(camera->GetMatView() * camera->GetMatProjection() * matViewport);

	return  Mat4::Transform(Vec3(position_.x, position_.y, 1.0f), matInverseVPV);
}
