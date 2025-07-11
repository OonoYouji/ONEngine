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

void Input::RegisterMonoFunctions() {
	gInputSystem_->RegisterMonoFunctions();
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

bool Input::PressGamepad(int _button) {
	return (gInputSystem_->gamepad_->state_.Gamepad.wButtons & static_cast<WORD>(_button)) != 0;
}

bool Input::TriggerGamepad(int _button) {
	return PressGamepad(_button) && (gInputSystem_->gamepad_->prevState_.Gamepad.wButtons & static_cast<WORD>(_button)) == 0;
}

bool Input::ReleaseGamepad(int _button) {
	return !PressGamepad(_button) && (gInputSystem_->gamepad_->prevState_.Gamepad.wButtons & static_cast<WORD>(_button)) != 0;
}

Vector2 Input::GetGamepadLeftThumb() {

	if (std::abs(gInputSystem_->gamepad_->state_.Gamepad.sThumbLX) != 8000 
		|| std::abs(gInputSystem_->gamepad_->state_.Gamepad.sThumbLY) != 8000) {
		return Vector2(
			static_cast<float>(gInputSystem_->gamepad_->state_.Gamepad.sThumbLX) / XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
			static_cast<float>(gInputSystem_->gamepad_->state_.Gamepad.sThumbLY) / XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		);
	}

	return Vector2::kZero;
}

Vector2 Input::GetGamepadRightThumb() {

	if (gInputSystem_->gamepad_->state_.Gamepad.sThumbRX != 0 || gInputSystem_->gamepad_->state_.Gamepad.sThumbRY != 0) {
		return Vector2(
			static_cast<float>(gInputSystem_->gamepad_->state_.Gamepad.sThumbRX) / XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
			static_cast<float>(gInputSystem_->gamepad_->state_.Gamepad.sThumbRY) / XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
		);
	}

	return Vector2::kZero;
}

const Vector2& Input::GetMousePosition() {
	return gInputSystem_->mouse_->position_;
}

const Vector2& Input::GetMouseVelocity() {
	return gInputSystem_->mouse_->velocity_;
}

Vector2 Input::GetImGuiImageMousePosition(const std::string& _imageName) {
	return gInputSystem_->mouse_->GetImGuiImageMousePosition(_imageName);
}
