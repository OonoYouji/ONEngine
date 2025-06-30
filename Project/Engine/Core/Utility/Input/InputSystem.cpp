#include "InputSystem.h"

/// externals
#include <mono/jit/jit.h>

/// engine
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Core/ImGui/ImGuiManager.h"
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Input.h"


InputSystem::InputSystem() {}
InputSystem::~InputSystem() {}

void InputSystem::Initialize(WindowManager* _windowManager, ImGuiManager* _imGuiManager) {

	windowManager_ = _windowManager;

	HRESULT hr = DirectInput8Create(
		_windowManager->GetMainWindow()->GetWNDCLASS().hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)(&directInput_),
		nullptr
	);
	Assert(SUCCEEDED(hr), "Failed to generate directInput.");


	keyboard_ = std::make_unique<Keyboard>();
	keyboard_->Initialize(directInput_.Get(), _windowManager);

	mouse_ = std::make_unique<Mouse>();
	mouse_->Initialize(directInput_.Get(), _windowManager, _imGuiManager);

	gamepad_ = std::make_unique<Gamepad>();
	gamepad_->Initialize(directInput_.Get(), _windowManager);
}

void InputSystem::Update() {

	keyboard_->Update(windowManager_->GetActiveWindow());
	mouse_->Update(windowManager_->GetActiveWindow());
	gamepad_->Update(windowManager_->GetActiveWindow());

}

void InputSystem::RegisterMonoFunctions() {
	mono_add_internal_call("Input::InternalTriggerKey", (void*)Input::TriggerKey);
	mono_add_internal_call("Input::InternalPressKey", (void*)Input::PressKey);
	mono_add_internal_call("Input::InternalReleaseKey", (void*)Input::ReleaseKey);

	mono_add_internal_call("Input::InternalTriggerGamepad", (void*)Input::TriggerGamepad);
	mono_add_internal_call("Input::InternalPressGamepad", (void*)Input::PressGamepad);
	mono_add_internal_call("Input::InternalReleaseGamepad", (void*)Input::ReleaseGamepad);
	mono_add_internal_call("Input::InternalGetGamepadThumb", (void*)InternalGetGamepadThumb);
}

void InternalGetGamepadThumb(int _axisIndex, float* _x, float* _y) {
	Vector2 v = {};
	switch (_axisIndex) {
	case 0: // Left
		v = Input::GetGamepadLeftThumb();
		break;
	case 1: // Right
		v = Input::GetGamepadRightThumb();
		break;
	}

	*_x = v.x;
	*_y = v.y;
}
