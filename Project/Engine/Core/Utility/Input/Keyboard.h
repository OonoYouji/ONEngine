#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

/// std
#include <cmath>
#include <array>

/// //////////////////////////////////////////////////
/// キーボード入力処理クラス
/// //////////////////////////////////////////////////
class Keyboard final {
	friend class Input;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Keyboard();
	~Keyboard();

	/// @brief 初期化
	/// @param _directInput DirectInputのポインタ
	/// @param _windowManager WindowManagerのポインタ
	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager);

	/// @brief 更新処理
	/// @param _window Windowのポインタ
	void Update(class Window* _window);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

	std::array<BYTE, 256> keys_;	///< 今フレームの入力
	std::array<BYTE, 256> preKeys_; ///< 前フレームの入力

};

