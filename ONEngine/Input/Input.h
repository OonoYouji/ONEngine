#pragma once

#include <wrl/client.h>

#include <memory>

#include <Keyboard.h>
#include <Mouse.h>
#include <Gamepad.h>

namespace ONE { class WinApp; }


/// ===================================================
/// 入力クラス
/// ===================================================
class Input final {
	Input() = default;
	~Input() = default;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	static Input* GetInsatnce();

	void Initialize(ONE::WinApp* winApp);

	void Finalize();

	void Begin();



	static bool PressKey(KeyCode keycode);		/// 入力中か
	static bool TriggerKey(KeyCode keycode);	/// 押した瞬間か
	static bool ReleaseKey(KeyCode keycode);	/// 離した瞬間か

	static bool PressMouse(MouseCode mouseCode);
	static bool TriggerMouse(MouseCode mouseCode);
	static bool ReleaseMouse(MouseCode mouseCode);

	static Vec2 MousePosition();
	static Vec2 MouseVelocity();
	static float MouseScroll();
	static Vec3 MouseRay(float distance);
	static Vec3 MouseNearPosition();
	static Vec3 MouseFarPosition();

	static bool PressPadButton(PadCode padCode);
	static bool TriggerPadButton(PadCode padCode);
	static bool ReleasePadButton(PadCode padCode);

	static bool PadState(XINPUT_STATE& out);
	static bool PadStatePrevious(XINPUT_STATE& out);
	static bool PadState(Pad pad);

	static void SetPadDeadZone(Pad pad, int32_t deadZoneL, int32_t deadZoneR);
	static Vector2 GetLeftStick();
	static Vector2 GetRightStick();
	static Vector2 GetLStick();
	static Vector2 GetRStick();
	static uint8_t GetLeftTrigger();
	static uint8_t GetRightTrigger();
	static uint8_t GetLTrigger();
	static uint8_t GetRTrigger();

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================

	static std::unique_ptr<Keyboard> keyboard_;
	static std::unique_ptr<Mouse> mouse_;
	static std::unique_ptr<Gamepad> pad_;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================
	
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
	ONE::WinApp* pWinApp_ = nullptr;

private:
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator= (const Input&) = delete;
	Input& operator= (Input&&) = delete;
};