#pragma once

#include <wrl/client.h>

#include <memory>

#include <Keyboard.h>
#include <Mouse.h>

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

private:

	/// ===================================================
	/// private : static objects
	/// ===================================================

	static std::unique_ptr<Keyboard> keyboard_;
	static std::unique_ptr<Mouse> mouse_;

private:

	/// ===================================================
	/// private : objects
	/// ===================================================
	
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;


private:
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator= (const Input&) = delete;
	Input& operator= (Input&&) = delete;
};