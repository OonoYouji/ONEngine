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
	Gamepad* gamepad = gInputSystem_->gamepad_.get();
	if (std::abs(gamepad->state_.Gamepad.sThumbLX) != gamepad->stickDeadZone_
		|| std::abs(gamepad->state_.Gamepad.sThumbLY) != gamepad->stickDeadZone_) {
		return Vector2(
			static_cast<float>(gamepad->state_.Gamepad.sThumbLX) / XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
			static_cast<float>(gamepad->state_.Gamepad.sThumbLY) / XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		);
	}

	return Vector2::kZero;
}

Vector2 Input::GetGamepadRightThumb() {
	Gamepad* gamepad = gInputSystem_->gamepad_.get();
	if (std::abs(gamepad->state_.Gamepad.sThumbRX) != gamepad->stickDeadZone_
		|| std::abs(gamepad->state_.Gamepad.sThumbRY) != gamepad->stickDeadZone_) {
		return Vector2(
			static_cast<float>(gamepad->state_.Gamepad.sThumbRX) / XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
			static_cast<float>(gamepad->state_.Gamepad.sThumbRY) / XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
		);
	}

	return Vector2::kZero;
}

float Input::GetMouseWheel() {
	return gInputSystem_->mouse_->wheel_;
}

const Vector2& Input::GetMousePosition() {
	return gInputSystem_->mouse_->position_;
}

const Vector2& Input::GetMouseVelocity() {
	return gInputSystem_->mouse_->velocity_;
}

const Vector2& Input::GetImGuiImageMousePosNormalized(const std::string& _imageName) {
	return gInputSystem_->mouse_->GetImGuiImageMousePosNormalized(_imageName);
}

const Vector2& Input::GetImGuiImagePos(const std::string& _imageName) {
	return gInputSystem_->mouse_->GetImGuiImagePos(_imageName);
}

const Vector2& Input::GetImGuiImageSize(const std::string& _imageName) {
	return gInputSystem_->mouse_->GetImGuiImageSize(_imageName);
}
