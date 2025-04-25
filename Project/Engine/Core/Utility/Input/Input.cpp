#include "Input.h"

/// engine
#include "InputSystem.h"

namespace {
	std::unique_ptr<InputSystem> gInputSystem_;
} /// namespace

void Input::Initialize(WindowManager* _windowManager, ImGuiManager* _imguiManager) {
	gInputSystem_ = std::make_unique<InputSystem>();
	gInputSystem_->Initialize(_windowManager, _imguiManager);
}

void Input::Update() {
	gInputSystem_->Update();
}

void Input::Finalize() {
	gInputSystem_.reset();
}


bool Input::PressKey(int _key) {
	return gInputSystem_->keyboard_->keys_[_key];
}

bool Input::TriggerKey(int _key) {
	return gInputSystem_->keyboard_->keys_[_key] && !gInputSystem_->keyboard_->preKeys_[_key];
}

bool Input::ReleaseKey(int _key) {
	return !gInputSystem_->keyboard_->keys_[_key] && gInputSystem_->keyboard_->preKeys_[_key];
}

bool Input::PressMouse(int _button) {
	return gInputSystem_->mouse_->state_.rgbButtons[_button];
}

bool Input::TriggerMouse(int _button) {
	return gInputSystem_->mouse_->state_.rgbButtons[_button] && !gInputSystem_->keyboard_->preKeys_[_button];
}

bool Input::ReleaseMouse(int _button) {
	return !gInputSystem_->mouse_->state_.rgbButtons[_button] && gInputSystem_->keyboard_->preKeys_[_button];
}

Vector2 Input::GetMousePosition() {
	return gInputSystem_->mouse_->position_;
}

Vector2 Input::GetMouseVelocity() {
	return gInputSystem_->mouse_->velocity_;
}

Vector2 Input::GetImGuiImageMousePosition() {
	return gInputSystem_->mouse_->GetImGuiImageMousePosition();
}
