#include "Mouse.h"


/// windows
#include <windows.h>

/// std
#include <cassert>
#include <cmath>

#include <Core/ONEngine.h>

#include "WindowManager/WinApp.h"
#include "Objects/Camera/Manager/CameraManager.h"
#include "Input.h"

namespace {
	bool IsWindowActive(HWND hwnd) {
		// 現在アクティブなウィンドウと比較
		return GetForegroundWindow() == hwnd;
	}

	void ConfineCursorToWindow(HWND hwnd) {
		if(IsWindowActive(hwnd)) {
			// ウィンドウがアクティブな場合、カーソルを制限
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);

			// クライアント座標をスクリーン座標に変換
			POINT topLeft = { clientRect.left, clientRect.top };
			POINT bottomRight = { clientRect.right, clientRect.bottom };
			ClientToScreen(hwnd, &topLeft);
			ClientToScreen(hwnd, &bottomRight);

			RECT clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
			ClipCursor(&clipRect); // カーソルの移動範囲を制限
		} else {
			// ウィンドウが非アクティブな場合、制限を解除
			ClipCursor(NULL);
		}
	}
}


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
		pWinApp_->GetHWND(), Input::sDWord_);
	assert(SUCCEEDED(hr));
}



void Mouse::Begin() {

	pWinApp_ = ONEngine::GetActiveWinApp();
	mouse_->SetCooperativeLevel(
		pWinApp_->GetHWND(), Input::sDWord_);

	preState_ = state_;

	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(state_), &state_);
	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(pWinApp_->GetHWND(), &mousePos);

	ConfineCursorToWindow(pWinApp_->GetHWND());

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
	Vec3 direction = Vec3::Normalize(farPos - nearPos);

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
