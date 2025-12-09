#include "InputSystem.h"

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Input.h"


InputSystem::InputSystem() = default;
InputSystem::~InputSystem() = default;

void InputSystem::Initialize(WindowManager* _windowManager, ImGuiManager* _imGuiManager) {
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

void InputSystem::Update() {
	keyboard_->Update(pWindowManager_->GetActiveWindow());
	mouse_->Update(pWindowManager_->GetActiveWindow());
	gamepad_->Update(pWindowManager_->GetActiveWindow());
}

void MONO_INTERNAL_METHOD::InternalGetGamepadThumb(int _axisIndex, float* _x, float* _y) {
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

void MONO_INTERNAL_METHOD::InternalGetMouseVelocity(float* _x, float* _y) {
	const Vector2& velocity = Input::GetMouseVelocity();
	*_x = velocity.x;
	*_y = velocity.y;
}

void MONO_INTERNAL_METHOD::InternalGetMousePosition(float* _x, float* _y) {
	const Vector2& position = Input::GetMousePosition();
	*_x = position.x;
	*_y = position.y;
}
