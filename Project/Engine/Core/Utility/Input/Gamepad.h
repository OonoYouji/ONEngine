#pragma once

#include "DInput.h"
/// directX12
#include <XInput.h>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"

/// ///////////////////////////////////////////////////
/// ゲームパッド入力処理クラス
/// ///////////////////////////////////////////////////
class Gamepad final {
	friend class Input;
public:

	/// @brief GamepadButton定数
	enum Button {
		DPadUp = XINPUT_GAMEPAD_DPAD_UP,
		DPadDown = XINPUT_GAMEPAD_DPAD_DOWN,
		DPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,
		DPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,
		Start = XINPUT_GAMEPAD_START,
		Back = XINPUT_GAMEPAD_BACK,
		LeftThumb = XINPUT_GAMEPAD_LEFT_THUMB,
		RightThumb = XINPUT_GAMEPAD_RIGHT_THUMB,
		LeftShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
		RightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,
		A = XINPUT_GAMEPAD_A,
		B = XINPUT_GAMEPAD_B,
		X = XINPUT_GAMEPAD_X,
		Y = XINPUT_GAMEPAD_Y
	};

	/// @brief Gamepadスティックの定数
	enum Axis {
		Left, Right
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Gamepad();
	~Gamepad();

	/// @brief 初期化
	/// @param _directInput DirectInputのポインタ
	/// @param _windowManager WindowManagerのポインタ
	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager);

	/// @brief 更新処理
	/// @param _window 現在のWindowのポインタ
	void Update(class Window* _window);


private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	ComPtr<IDirectInputDevice8> gamepadDevice_;
	
	XINPUT_STATE state_;
	XINPUT_STATE prevState_;

	int stickDeadZone_ = 8000; // スティックのデッドゾーン
};

