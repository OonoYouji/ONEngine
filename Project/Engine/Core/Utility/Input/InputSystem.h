#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

/// ===================================================
/// 入力処理クラス
/// ===================================================
class InputSystem final {
	friend class Input;
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	InputSystem();
	~InputSystem();

	void Initialize(class WindowManager* _windowManager, class ImGuiManager* _imGuiManager);
	void Update();

	void RegisterMonoFunctions();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	ComPtr<IDirectInput8>     directInput_;
	std::unique_ptr<Keyboard> keyboard_;
	std::unique_ptr<Mouse>    mouse_;
	std::unique_ptr<Gamepad>  gamepad_;

	class WindowManager*      windowManager_ = nullptr;
};

