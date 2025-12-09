#pragma once

/// std
#include <memory>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

/// //////////////////////////////////////////////////
/// 入力処理クラス
/// //////////////////////////////////////////////////
class InputSystem final {
	friend class Input;
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	InputSystem();
	~InputSystem();

	/// @brief 初期化
	/// @param _windowManager WindowManagerのポインタ
	/// @param _imGuiManager  ImGuiManagerのポインタ
	void Initialize(class WindowManager* _windowManager, class ImGuiManager* _imGuiManager);

	/// @brief 更新処理
	void Update();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	/// ----- other class ----- ///
	class WindowManager* pWindowManager_ = nullptr;

	ComPtr<IDirectInput8>     directInput_;
	std::unique_ptr<Keyboard> keyboard_;
	std::unique_ptr<Mouse>    mouse_;
	std::unique_ptr<Gamepad>  gamepad_;

};

namespace MONO_INTERNAL_METHOD {
	void InternalGetGamepadThumb(int _axisIndex, float* _x, float* _y);
	void InternalGetMouseVelocity(float* _x, float* _y);
	void InternalGetMousePosition(float* _x, float* _y);
}