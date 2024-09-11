#pragma once

#include <wrl/client.h>
#include <XInput.h>
#include <dinput.h>
#include <vector>
#include <cmath>
#include <Vector2.h>
#pragma comment(lib, "xinput.lib")


namespace ONE { class WinApp; }

/// ===================================================
/// GamePadの入力ステート
/// ===================================================
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



/// ===================================================
/// GamePad一個当たりの情報
/// ===================================================
struct Pad {
	Microsoft::WRL::ComPtr<IDirectInputDevice8> device_;
	int32_t deadZoneL_;
	int32_t deadZoneR_;
	XINPUT_STATE state_;
	XINPUT_STATE statePre_;
};



/// ===================================================
/// GamePadクラス
/// ===================================================
class Gamepad final {
public:

	Gamepad();
	~Gamepad();

	/// ===================================================
	/// public : methods
	/// ===================================================

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(IDirectInput8* devInput, ONE::WinApp* winApp);

	/// <summary>
	/// フレームの最初に行う
	/// </summary>
	void Begin();



	/// <summary>
	/// デッドゾーンの設定
	/// </summary>
	/// <param name="pad"></param>
	/// <param name="deadZoneL"></param>
	/// <param name="deadZoneR"></param>
	void SetPadDeadZone(Pad& pad, int32_t deadZoneL, int32_t deadZoneR);


	/// <summary>
	/// 今フレームのpad stateを得る
	/// </summary>
	/// <param name="out"></param>
	/// <returns></returns>
	bool GetPadState(XINPUT_STATE& out)const;

	/// <summary>
	/// 前フレームのpad stateを得る
	/// </summary>
	/// <param name="out"></param>
	/// <returns></returns>
	bool GetPadStatePrevious(XINPUT_STATE& out)const;


	/// <summary>
	/// 今フレームのpad stateを得る
	/// </summary>
	/// <param name="out"></param>
	/// <returns></returns>
	bool GetPadState(Pad& out)const;
	


	/// <summary>
	/// 押しているか
	/// </summary>
	/// <param name="code"></param>
	/// <returns></returns>
	bool Press(PadCode code) const;

	/// <summary>
	/// 押した瞬間か
	/// </summary>
	/// <param name="code"></param>
	/// <returns></returns>
	bool Trigger(PadCode code) const;

	/// <summary>
	/// 離した瞬間か
	/// </summary>
	/// <param name="code"></param>
	/// <returns></returns>
	bool Release(PadCode code) const;



	/// <summary>
	/// 左スティックの角度
	/// </summary>
	/// <returns></returns>
	Vector2 GetLeftStick() const;

	/// <summary>
	/// 右スティックの角度
	/// </summary>
	/// <returns></returns>
	Vector2 GetRightStick() const;



	/// <summary>
	/// LTの押し込み度
	/// </summary>
	/// <returns></returns>
	BYTE GetLeftTrigger() const;

	/// <summary>
	/// RTの押し込み度
	/// </summary>
	/// <returns></returns>
	BYTE GetRightTrigger() const;
	

	bool IsLStickStartMoving() const { return isLStickMoving_ && !isPrevLStickMoving_; }
	bool IsRStickStartMoving() const { return isRStickMoving_ && !isPrevRStickMoving_; }

private:

	/// ===================================================
	/// private : methods
	/// ===================================================

	Vector2 ApplyDeadZone(int32_t x, int32_t y, int32_t deadZone) const;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	Pad pad_;
	bool isLStickMoving_ = false;
	bool isPrevLStickMoving_ = false;

	bool isRStickMoving_ = false;
	bool isPrevRStickMoving_ = false;




private:
	Gamepad(const Gamepad&) = delete;
	Gamepad(Gamepad&&) = delete;
	Gamepad& operator=(const Gamepad&) = delete;
	Gamepad& operator=(Gamepad&&) = delete;
};