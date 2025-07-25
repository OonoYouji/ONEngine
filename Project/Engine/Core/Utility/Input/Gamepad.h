#pragma once

/// directX12
#include <wrl/client.h>
#include <XInput.h>
#include <dinput.h>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"

/// ///////////////////////////////////////////////////
/// ゲームパッド入力処理クラス
/// ///////////////////////////////////////////////////
class Gamepad final {
	friend class Input;
public:

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

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	Gamepad();
	~Gamepad();

	void Initialize(IDirectInput8* _directInput, class WindowManager* _windowManager);
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

