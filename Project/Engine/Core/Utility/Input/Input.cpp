#include "Input.h"

/// engine
#include "InputSystem.h"

namespace {
	std::unique_ptr<InputSystem> gInputSystem_;
} /// namespace

void Input::Initialize(WindowManager* _windowManager) {
	gInputSystem_ = std::make_unique<InputSystem>();
	gInputSystem_->Initialize(_windowManager);
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
