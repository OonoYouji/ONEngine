#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

/// std
#include <cmath>
#include <array>

/// ===================================================
/// キーボード入力処理クラス
/// ===================================================
class Keyboard final {
	friend class Input;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	Keyboard();
	~Keyboard();

	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager);
	void Update(class Window* _window);

private:

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

	std::array<BYTE, 256> keys_;	///< 今フレームの入力
	std::array<BYTE, 256> preKeys_; ///< 前フレームの入力

};

