#include "Input.h"

using namespace ONEngine;

/// engine
#include "InputSystem.h"

namespace {
	std::unique_ptr<InputSystem> gInputSystem_;
} /// namespace

/**
 * @brief 入力システムを初期化します。
 */
void Input::Initialize(WindowManager* _windowManager, Editor::ImGuiManager* _imguiManager) {
	gInputSystem_ = std::make_unique<InputSystem>();
	gInputSystem_->Initialize(_windowManager, _imguiManager);
}

/**
 * @brief 毎フレーム呼び出され、各入力デバイスの入力状態を更新します。
 */
void Input::Update() {
	gInputSystem_->Update();
}

/**
 * @brief 入力システムの終了処理（デバイスの破棄など）を行います。
 */
void Input::Finalize() {
	gInputSystem_.reset();
}

/**
 * @brief 指定したキーが押されているか判定します。
 */
bool Input::PressKey(int _key) {
	return gInputSystem_->keyboard_->keys_[_key];
}

/**
 * @brief 指定したキーが今フレームで押された瞬間（トリガー）か判定します。
 */
bool Input::TriggerKey(int _key) {
	return gInputSystem_->keyboard_->keys_[_key] && !gInputSystem_->keyboard_->preKeys_[_key];
}

/**
 * @brief 指定したキーが今フレームで離された瞬間か判定します。
 */
bool Input::ReleaseKey(int _key) {
	return !gInputSystem_->keyboard_->keys_[_key] && gInputSystem_->keyboard_->preKeys_[_key];
}

/**
 * @brief 指定したマウスボタンが押されているか判定します。
 */
bool Input::PressMouse(int _button) {
	return gInputSystem_->mouse_->state_.rgbButtons[_button];
}

/**
 * @brief 指定したマウスボタンが今フレームで押された瞬間か判定します。
 */
bool Input::TriggerMouse(int _button) {
	return gInputSystem_->mouse_->state_.rgbButtons[_button] && !gInputSystem_->mouse_->preState_.rgbButtons[_button];
}

/**
 * @brief 指定したマウスボタンが今フレームで離された瞬間か判定します。
 */
bool Input::ReleaseMouse(int _button) {
	return !gInputSystem_->mouse_->state_.rgbButtons[_button] && gInputSystem_->mouse_->preState_.rgbButtons[_button];
}

/**
 * @brief ゲームパッドのボタンが押されているか判定します。
 */
bool Input::PressGamepad(int _button) {
	return (gInputSystem_->gamepad_->state_.Gamepad.wButtons & static_cast<WORD>(_button)) != 0;
}

/**
 * @brief ゲームパッドのボタンが今フレームで押された瞬間（トリガー）か判定します。
 */
bool Input::TriggerGamepad(int _button) {
	return PressGamepad(_button) && (gInputSystem_->gamepad_->prevState_.Gamepad.wButtons & static_cast<WORD>(_button)) == 0;
}

/**
 * @brief ゲームパッドのボタンが今フレームで離された瞬間か判定します。
 */
bool Input::ReleaseGamepad(int _button) {
	return !PressGamepad(_button) && (gInputSystem_->gamepad_->prevState_.Gamepad.wButtons & static_cast<WORD>(_button)) != 0;
}

/**
 * @brief ゲームパッドの左スティックの倒し量ベクトルを取得します。
 */
Vector2 Input::GetGamepadLeftThumb() {
	Gamepad* gamepad = gInputSystem_->gamepad_.get();
	if (std::abs(gamepad->state_.Gamepad.sThumbLX) != gamepad->stickDeadZone_
		|| std::abs(gamepad->state_.Gamepad.sThumbLY) != gamepad->stickDeadZone_) {
		return Vector2(
			static_cast<float>(gamepad->state_.Gamepad.sThumbLX) / XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
			static_cast<float>(gamepad->state_.Gamepad.sThumbLY) / XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		);
	}

	return Vector2::Zero;
}

/**
 * @brief ゲームパッドの右スティックの倒し量ベクトルを取得します。
 */
Vector2 Input::GetGamepadRightThumb() {
	Gamepad* gamepad = gInputSystem_->gamepad_.get();
	if (std::abs(gamepad->state_.Gamepad.sThumbRX) != gamepad->stickDeadZone_
		|| std::abs(gamepad->state_.Gamepad.sThumbRY) != gamepad->stickDeadZone_) {
		return Vector2(
			static_cast<float>(gamepad->state_.Gamepad.sThumbRX) / XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
			static_cast<float>(gamepad->state_.Gamepad.sThumbRY) / XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
		);
	}

	return Vector2::Zero;
}

/**
 * @brief マウスホイールの回転量を取得します。
 */
float Input::GetMouseWheel() {
	return gInputSystem_->mouse_->wheel_;
}

/**
 * @brief マウスの現在のスクリーン座標を取得します。
 */
const Vector2& Input::GetMousePosition() {
	return gInputSystem_->mouse_->position_;
}

/**
 * @brief マウスのフレーム移動量（速度）を取得します。
 */
const Vector2& Input::GetMouseVelocity() {
	return gInputSystem_->mouse_->velocity_;
}

/**
 * @brief 指定された ImGui 画像ウインドウ内における、マウス位置を正規化した座標で取得します。
 */
const Vector2& Input::GetImGuiImageMousePosNormalized(const std::string& _imageName) {
	return gInputSystem_->mouse_->GetImGuiImageMousePosNormalized(_imageName);
}

/**
 * @brief 指定された ImGui 画像ウインドウのスクリーン座標を取得します。
 */
const Vector2& Input::GetImGuiImagePos(const std::string& _imageName) {
	return gInputSystem_->mouse_->GetImGuiImagePos(_imageName);
}

/**
 * @brief 指定された ImGui 画像ウインドウのサイズを取得します。
 */
const Vector2& Input::GetImGuiImageSize(const std::string& _imageName) {
	return gInputSystem_->mouse_->GetImGuiImageSize(_imageName);
}
