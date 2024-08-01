#include <Keyboard.h>


#include <cassert>

#include <WinApp.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")






Keyboard::Keyboard() {}
Keyboard::~Keyboard() {}


/// ===================================================
/// 初期化
/// ===================================================
void Keyboard::Initialize(IDirectInput8* directInput, ONE::WinApp* winApp) {
	HRESULT hr = directInput->CreateDevice(
		GUID_SysKeyboard, &keyboard_, NULL);

	assert(SUCCEEDED(hr) && "キーボードデバイスの生成に失敗しました");

	///- 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard); //- 標準形式
	assert(SUCCEEDED(hr));

	///- 排他制御レベルのセット
	// DISCL_FOREGROUND   : 画面が手前にある場合のみ入力を受け付ける
	// DISCL_NONEXCLUSIVE : デバイスをこのアプリだけで占有しない
	// DISCL_NOWINKEY     : Windowsキーを無効にする

	hr = keyboard_->SetCooperativeLevel(
		winApp->GetHWND(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(hr));
}


/// ===================================================
/// 入力を得る
/// ===================================================
void Keyboard::Begin() {
	///- キーボード情報の取得開始
	keyboard_->Acquire();

	preKeys_ = keys_;

	keyboard_->GetDeviceState(sizeof(keys_), keys_.data());

}


/// ===================================================
/// キー入力の取得
/// ===================================================
bool Keyboard::Push(KeyCode keycode) const {
	return keys_[static_cast<BYTE>(keycode)];
}

bool Keyboard::Trigger(KeyCode keycode) const {
	return Push(keycode) && !preKeys_[static_cast<uint8_t>(keycode)];
}

bool Keyboard::Release(KeyCode keycode) const {
	return !Push(keycode) && preKeys_[static_cast<uint8_t>(keycode)];
}
