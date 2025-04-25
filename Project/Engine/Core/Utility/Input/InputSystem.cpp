#include "InputSystem.h"

/// engine
#include "Engine/Core/Window/WindowManager.h"
#include "Engine/Core/Utility/Tools/Assert.h"

InputSystem::InputSystem() {}
InputSystem::~InputSystem() {}

void InputSystem::Initialize(WindowManager* _windowManager) {

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
	mouse_->Initialize(directInput_.Get(), _windowManager);
}

void InputSystem::Update() {

	keyboard_->Update(windowManager_->GetActiveWindow());
	mouse_->Update(windowManager_->GetActiveWindow());

}
