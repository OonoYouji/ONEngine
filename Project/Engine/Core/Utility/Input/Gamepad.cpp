#include "Gamepad.h"

/// comment
#pragma comment(lib, "xinput.lib")

/// engine
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Core/Utility/Utility.h"

Gamepad::Gamepad() {}
Gamepad::~Gamepad() {}

void Gamepad::Initialize(IDirectInput8* _directInput, WindowManager* _windowManager) {
	HRESULT hr = _directInput->CreateDevice(
		GUID_SysKeyboard, &gamepadDevice_, NULL);

	Assert(SUCCEEDED(hr), "ゲームパッドデバイスの生成に失敗しました");

	/// 入力データ形式のセット
	hr = gamepadDevice_->SetDataFormat(&c_dfDIKeyboard); ///< 標準形式
	Assert(SUCCEEDED(hr), "ゲームパッドデバイスの生成に失敗しました");

	/// 排他制御レベルのセット
	/*
		DISCL_FOREGROUND   : 画面が手前にある場合のみ入力を受け付ける
		DISCL_NONEXCLUSIVE : デバイスをこのアプリだけで占有しない
		DISCL_NOWINKEY     : Windowsキーを無効にする
	*/

	hr = gamepadDevice_->SetCooperativeLevel(
		_windowManager->GetMainWindow()->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	Assert(SUCCEEDED(hr), "ゲームパッドデバイスの生成に失敗しました");
}

void Gamepad::Update(Window* ) {

	/// 前フレームの状態を保存
	prevState_ = state_;

	/// 現在の状態を取得
	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	XInputGetState(0, &state_);
}
