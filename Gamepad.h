#pragma once

#include <wrl/client.h>
#include <XInput.h>
#include <dinput.h>
#include <vector>
#include <cmath>
#include <Vector2.h>
#pragma comment(lib, "xinput.lib")


namespace ONE { class WinApp; }

enum class PadCode : WORD {
	Up = XINPUT_GAMEPAD_DPAD_UP,
	Down = XINPUT_GAMEPAD_DPAD_DOWN,
	Left = XINPUT_GAMEPAD_DPAD_LEFT,
	Right = XINPUT_GAMEPAD_DPAD_RIGHT,

	Start = XINPUT_GAMEPAD_START,
	Back = XINPUT_GAMEPAD_BACK,

	LeftStick = XINPUT_GAMEPAD_LEFT_THUMB, LStick = XINPUT_GAMEPAD_LEFT_THUMB,
	RightStick = XINPUT_GAMEPAD_RIGHT_THUMB, RStick = XINPUT_GAMEPAD_RIGHT_THUMB,

	LeftShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
	RightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,

	A = XINPUT_GAMEPAD_A,
	B = XINPUT_GAMEPAD_B,
	X = XINPUT_GAMEPAD_X,
	Y = XINPUT_GAMEPAD_Y
};

struct Pad {
	Microsoft::WRL::ComPtr<IDirectInputDevice8> device_;
	int32_t deadZoneL_;
	int32_t deadZoneR_;
	XINPUT_STATE state_;
	XINPUT_STATE statePre_;
};

class Gamepad final {
public:
	Gamepad();
	~Gamepad();


	void Initialize(IDirectInput8* devInput, ONE::WinApp* winApp);

	void Begin();

	void SetPadDeadZone(Pad pad, int32_t deadZoneL, int32_t deadZoneR);

	bool GetPadState(XINPUT_STATE& out)const;
	bool GetPadStatePrevious(XINPUT_STATE& out)const;

	bool GetPadState(Pad out)const;

	bool Press(PadCode code) const;
	bool Trigger(PadCode code) const;
	bool Release(PadCode code) const;

	Vector2 GetLeftStick() const;
	Vector2 GetRightStick() const;

	BYTE GetLeftTrigger() const;
	BYTE GetRightTrigger() const;

private:

	Vector2 ApplyDeadZone(int32_t x, int32_t y, int32_t deadZone) const;

private:

	Pad pad_;

private:
	Gamepad(const Gamepad&) = delete;
	Gamepad(Gamepad&&) = delete;
	Gamepad& operator=(const Gamepad&) = delete;
	Gamepad& operator=(Gamepad&&) = delete;
};