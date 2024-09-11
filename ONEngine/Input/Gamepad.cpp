#include "Gamepad.h"
#include <WinApp.h>
#include < stdexcept >

Gamepad::Gamepad() : pad_({ nullptr,8000,8000,{},{} }) {}

Gamepad::~Gamepad() {}

void Gamepad::Initialize(IDirectInput8* devInput, ONE::WinApp* winApp) {
	//// DirectInput デバイスの作成
	//HRESULT hr = devInput->CreateDevice(GUID_Joystick, &pad_.device_, nullptr);
	//if (FAILED(hr)) {
	//	throw std::runtime_error("Failed to create DirectInput device for gamepad.");
	//}

	//// デバイスの設定
	//hr = pad_.device_->SetDataFormat(&c_dfDIJoystick2);
	//if (FAILED(hr)) {
	//	throw std::runtime_error("Failed to set data format for gamepad.");
	//}

	//hr = pad_.device_->SetCooperativeLevel(winApp->GetHWND(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	//if (FAILED(hr)) {
	//	throw std::runtime_error("Failed to set cooperative level for gamepad.");
	//}

	//// デバイスを取得
	//hr = pad_.device_->Acquire();
	//if (FAILED(hr)) {
	//	throw std::runtime_error("Failed to acquire gamepad device.");
	//}
}



void Gamepad::Begin() {
	// 前回の状態を保存
	pad_.statePre_ = pad_.state_;

	// 現在の状態を取得
	ZeroMemory(&pad_.state_, sizeof(XINPUT_STATE));
	XInputGetState(0, &pad_.state_);

	isLStickMoving_ = ApplyDeadZone(pad_.state_.Gamepad.sThumbLX, pad_.state_.Gamepad.sThumbLY,pad_.deadZoneL_).Lenght() != 0.0f;
	isPrevLStickMoving_ = ApplyDeadZone(pad_.statePre_.Gamepad.sThumbLX, pad_.statePre_.Gamepad.sThumbLY,pad_.deadZoneL_).Lenght() != 0.0f;

	isRStickMoving_ = ApplyDeadZone(pad_.state_.Gamepad.sThumbRX, pad_.state_.Gamepad.sThumbRY,pad_.deadZoneL_).Lenght() != 0.0f;
	isPrevRStickMoving_ = ApplyDeadZone(pad_.statePre_.Gamepad.sThumbRX, pad_.statePre_.Gamepad.sThumbRY,pad_.deadZoneL_).Lenght() != 0.0f;


}

void Gamepad::SetPadDeadZone(Pad& pad, int32_t deadZoneL, int32_t deadZoneR) {
	pad_.deadZoneL_ = deadZoneL;
	pad_.deadZoneR_ = deadZoneR;
	pad = pad_;
}



bool Gamepad::GetPadState(XINPUT_STATE& out) const {
	out = pad_.state_;
	return true;
}

bool Gamepad::GetPadStatePrevious(XINPUT_STATE& out) const {
	out = pad_.statePre_;
	return true;
}

bool Gamepad::GetPadState(Pad& out) const {
	out = pad_;
	return true;
}



bool Gamepad::Press(PadCode code) const {
	return (pad_.state_.Gamepad.wButtons & static_cast<WORD>(code)) != 0;
}

bool Gamepad::Trigger(PadCode code) const {
	return (!(pad_.statePre_.Gamepad.wButtons & static_cast<WORD>(code)) &&
			(pad_.state_.Gamepad.wButtons & static_cast<WORD>(code)));
}

bool Gamepad::Release(PadCode code) const {
	return ((pad_.statePre_.Gamepad.wButtons & static_cast<WORD>(code)) &&
			!(pad_.state_.Gamepad.wButtons & static_cast<WORD>(code)));
}



Vector2 Gamepad::GetLeftStick() const {
	return ApplyDeadZone(pad_.state_.Gamepad.sThumbLX, pad_.state_.Gamepad.sThumbLY, pad_.deadZoneL_);
}

Vector2 Gamepad::GetRightStick() const {
	return ApplyDeadZone(pad_.state_.Gamepad.sThumbRX, pad_.state_.Gamepad.sThumbRY, pad_.deadZoneR_);
}

BYTE Gamepad::GetLeftTrigger() const {
	return pad_.state_.Gamepad.bLeftTrigger;
}

BYTE Gamepad::GetRightTrigger() const {
	return pad_.state_.Gamepad.bRightTrigger;
}

Vector2 Gamepad::ApplyDeadZone(int32_t x, int32_t y, int32_t deadZone) const {
	Vector2 stick;

	float magnitude = sqrtf(static_cast<float>(x) * static_cast<float>(x) + static_cast<float>(y) * static_cast<float>(y));
	if(magnitude < deadZone) {
		stick.x = 0.0f;
		stick.y = 0.0f;
	} else {
		stick.x = static_cast<float>(x);
		stick.y = static_cast<float>(y);
	}

	return stick;
}
