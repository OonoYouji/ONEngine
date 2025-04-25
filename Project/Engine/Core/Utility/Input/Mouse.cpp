#include "Mouse.h"

/// engine
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Window/WindowManager.h"

void Mouse::Initialize(IDirectInput8* _directInput, WindowManager* _windowManager) {
	HRESULT hr = _directInput->CreateDevice(
		GUID_SysKeyboard, &mouse_, NULL);

	Assert(SUCCEEDED(hr), "マウスデバイスの生成に失敗しました");

	/// 入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIKeyboard); ///< 標準形式
	Assert(SUCCEEDED(hr), "マウスデバイスの生成に失敗しました");

	/// 排他制御レベルのセット
	/*
		DISCL_FOREGROUND   : 画面が手前にある場合のみ入力を受け付ける
		DISCL_NONEXCLUSIVE : デバイスをこのアプリだけで占有しない
		DISCL_NOWINKEY     : Windowsキーを無効にする
	*/

	hr = mouse_->SetCooperativeLevel(
		_windowManager->GetMainWindow()->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	Assert(SUCCEEDED(hr), "マウスデバイスの生成に失敗しました");
}

void Mouse::Update(Window* _window) {
	mouse_->SetCooperativeLevel(
		_window->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);

	/// キーボード情報の取得開始
	preState_ = state_; ///< 前フレームの入力を保存

	mouse_->Acquire();
	mouse_->GetDeviceState(sizeof(state_), &state_);
	POINT mousePos{};
	GetCursorPos(&mousePos);
	ScreenToClient(_window->GetHwnd(), &mousePos);

	position_ = Vector2(
		static_cast<float>(mousePos.x),
		static_cast<float>(mousePos.y)
	);

	velocity_ = Vector2(
		static_cast<float>(state_.lX),
		static_cast<float>(state_.lY)
	);
}
