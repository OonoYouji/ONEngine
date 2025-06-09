#pragma once

/// engine
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

/// ===================================================
/// 入力処理クラス
/// ===================================================
class Input final {
	friend class GameFramework;

	static void Initialize(class WindowManager* _windowManager, class ImGuiManager* _imguiManager);
	static void Update();
	static void Finalize();

public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	/* --- keyboard --- */
	static bool PressKey(int _key);
	static bool TriggerKey(int _key);
	static bool ReleaseKey(int _key);

	
	/* --- mouse --- */
	static bool PressMouse(int _button);
	static bool TriggerMouse(int _button);
	static bool ReleaseMouse(int _button);

	static const Vector2& GetMousePosition();
	static const Vector2& GetMouseVelocity();
	static Vector2 GetImGuiImageMousePosition(const std::string& _imageName);


	/* --- gamepad --- */
	static bool PressGamepad(int _button);
	static bool TriggerGamepad(int _button);
	static bool ReleaseGamepad(int _button);

	static Vector2 GetGamepadLeftThumb();

};

