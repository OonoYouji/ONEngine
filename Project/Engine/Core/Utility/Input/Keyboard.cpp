#include "Keyboard.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Window/WindowManager.h"

/// comment
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


Keyboard::Keyboard() = default;
Keyboard::~Keyboard() = default;

/**
 * @brief キーボード入力デバイスの初期化およびデータフォーマット、協調レベルの設定を行います。
 */
void Keyboard::Initialize(IDirectInput8* _directInput, WindowManager* _windowManager) {
	HRESULT hr = _directInput->CreateDevice(
		GUID_SysKeyboard, &keyboard_, NULL);

	Assert(SUCCEEDED(hr), "キーボードデバイスの生成に失敗しました");

	/// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard); ///< 標準形式
	Assert(SUCCEEDED(hr), "キーボードデバイスの生成に失敗しました");

	/*	/// 排他制御レベルのセット
	    DISCL_FOREGROUND   : 画面が手前にある場合のみ入力を受け付ける
	    DISCL_NONEXCLUSIVE : デバイスをこのアプリだけで占有しない
	    DISCL_NOWINKEY     : Windowsキーを無効にする
	*/

	hr = keyboard_->SetCooperativeLevel(
		_windowManager->GetMainWindow()->GetHwnd(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	Assert(SUCCEEDED(hr), "キーボードデバイスの生成に失敗しました");
}


/**
 * @brief 毎フレームキーボードの最新のキー状態を取得し、前フレームの状態を保存します。
 */
void Keyboard::Update(Window* _window) {
	keyboard_->SetCooperativeLevel(
		_window->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);

	/// キーボード情報の取得開始
	keyboard_->Acquire();

	preKeys_ = keys_; ///< 前フレームの入力を保存

	keyboard_->GetDeviceState(sizeof(keys_), keys_.data());
}
