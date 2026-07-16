#include "InputSystem.h"

using namespace ONEngine;

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Input.h"

/// editor
#include "Engine/Editor/Manager/ImGuiManager.h"

InputSystem::InputSystem() = default;
InputSystem::~InputSystem() = default;

/**
 * @brief DirectInputの生成および各入力デバイスのインスタンス生成・初期化を行います。
 */
void InputSystem::Initialize(WindowManager* _windowManager, Editor::ImGuiManager* _imGuiManager) {
	pWindowManager_ = _windowManager;


	HRESULT hr = DirectInput8Create(
		pWindowManager_->GetMainWindow()->GetWNDCLASS().hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)(&directInput_),
		nullptr
	);
	Assert(SUCCEEDED(hr), "Failed to generate directInput.");


	/// ---------------------------------------------------
	/// 各入力デバイスの初期化
	/// ---------------------------------------------------

	keyboard_ = std::make_unique<Keyboard>();
	keyboard_->Initialize(directInput_.Get(), pWindowManager_);

	mouse_ = std::make_unique<Mouse>();
	mouse_->Initialize(directInput_.Get(), pWindowManager_, _imGuiManager);

	gamepad_ = std::make_unique<Gamepad>();
	gamepad_->Initialize(directInput_.Get(), pWindowManager_);
}

/**
 * @brief 毎フレーム呼び出され、全入力デバイスのデータをポーリングして状態を更新します。
 */
void InputSystem::Update() {
	keyboard_->Update(pWindowManager_->GetActiveWindow());
	mouse_->Update(pWindowManager_->GetActiveWindow());
	gamepad_->Update(pWindowManager_->GetActiveWindow());
}

/**
 * @brief C#（Mono）側からゲームパッドの入力値を取得するためのバインド関数。
 */
void MonoInternalMethods::InternalGetGamepadThumb(int _axisIndex, float* _x, float* _y) {
	Vector2 v = {};
	switch (_axisIndex) {
	case Gamepad::Axis::Left: // Left
		v = Input::GetGamepadLeftThumb();
		break;
	case Gamepad::Axis::Right: // Right
		v = Input::GetGamepadRightThumb();
		break;
	}

	*_x = v.x;
	*_y = v.y;
}

/**
 * @brief C#（Mono）側からマウスのフレーム移動量を取得するためのバインド関数。
 */
void MonoInternalMethods::InternalGetMouseVelocity(float* _x, float* _y) {
	const Vector2& velocity = Input::GetMouseVelocity();
	*_x = velocity.x;
	*_y = velocity.y;
}

/**
 * @brief C#（Mono）側からマウスのスクリーン座標を取得するためのバインド関数。
 */
void MonoInternalMethods::InternalGetMousePosition(float* _x, float* _y) {
	const Vector2& position = Input::GetMousePosition();
	*_x = position.x;
	*_y = position.y;
}

/**
 * @brief C#（Mono）側からマウスホイールの回転量を取得するためのバインド関数。
 */
void ONEngine::MonoInternalMethods::InternalGetMouseWheel(float* _wheel) {
	float wheel = Input::GetMouseWheel();
	if(_wheel) {
		*_wheel = wheel;
	}
}
