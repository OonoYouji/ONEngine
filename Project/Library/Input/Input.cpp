#include "Input.h"

#include <cassert>

#include "WindowManager/WinApp.h"

#include "ImGuiManager/ImGuiManager.h"




namespace {

	// KeyCodeからキー名を取得
	const char* GetKeyName(KeyCode key) {
		switch(key) {
		case KeyCode::A: return "A";
		case KeyCode::B: return "B";
		case KeyCode::C: return "C";
		case KeyCode::D: return "D";
		case KeyCode::E: return "E";
		case KeyCode::F: return "F";
		case KeyCode::G: return "G";
		case KeyCode::H: return "H";
		case KeyCode::I: return "I";
		case KeyCode::J: return "J";
		case KeyCode::K: return "K";
		case KeyCode::L: return "L";
		case KeyCode::M: return "M";
		case KeyCode::N: return "N";
		case KeyCode::O: return "O";
		case KeyCode::P: return "P";
		case KeyCode::Q: return "Q";
		case KeyCode::R: return "R";
		case KeyCode::S: return "S";
		case KeyCode::T: return "T";
		case KeyCode::U: return "U";
		case KeyCode::V: return "V";
		case KeyCode::W: return "W";
		case KeyCode::X: return "X";
		case KeyCode::Y: return "Y";
		case KeyCode::Z: return "Z";
		case KeyCode::Alpha0: return "0";
		case KeyCode::Alpha1: return "1";
		case KeyCode::Alpha2: return "2";
		case KeyCode::Alpha3: return "3";
		case KeyCode::Alpha4: return "4";
		case KeyCode::Alpha5: return "5";
		case KeyCode::Alpha6: return "6";
		case KeyCode::Alpha7: return "7";
		case KeyCode::Alpha8: return "8";
		case KeyCode::Alpha9: return "9";
		case KeyCode::Keypad0: return "Keypad 0";
		case KeyCode::Keypad1: return "Keypad 1";
		case KeyCode::Keypad2: return "Keypad 2";
		case KeyCode::Keypad3: return "Keypad 3";
		case KeyCode::Keypad4: return "Keypad 4";
		case KeyCode::Keypad5: return "Keypad 5";
		case KeyCode::Keypad6: return "Keypad 6";
		case KeyCode::Keypad7: return "Keypad 7";
		case KeyCode::Keypad8: return "Keypad 8";
		case KeyCode::Keypad9: return "Keypad 9";
		case KeyCode::KeypadEnter: return "Keypad Enter";
		case KeyCode::F1: return "F1";
		case KeyCode::F2: return "F2";
		case KeyCode::F3: return "F3";
		case KeyCode::F4: return "F4";
		case KeyCode::F5: return "F5";
		case KeyCode::F6: return "F6";
		case KeyCode::F7: return "F7";
		case KeyCode::F8: return "F8";
		case KeyCode::F9: return "F9";
		case KeyCode::F10: return "F10";
		case KeyCode::F11: return "F11";
		case KeyCode::F12: return "F12";
		case KeyCode::UpArrow: return "Up Arrow";
		case KeyCode::DownArrow: return "Down Arrow";
		case KeyCode::LeftArrow: return "Left Arrow";
		case KeyCode::RightArrow: return "Right Arrow";
		case KeyCode::LeftShift: return "Left Shift";
		case KeyCode::RightShift: return "Right Shift";
		case KeyCode::LeftControl: return "Left Control";
		case KeyCode::RightControl: return "Right Control";
		case KeyCode::LeftAlt: return "Left Alt";
		case KeyCode::RightAlt: return "Right Alt";
		case KeyCode::Colon: return "Colon";
		case KeyCode::SemiColon: return "SemiColon";
		case KeyCode::Slash: return "Slash";
		case KeyCode::BackSlash: return "BackSlash";
		case KeyCode::Comma: return "Comma";
		case KeyCode::Period: return "Period";
		case KeyCode::Minus: return "Minus";
		case KeyCode::Equals: return "Equals";
		case KeyCode::Grave: return "Grave";
		case KeyCode::LeftBracket: return "Left Bracket";
		case KeyCode::RightBracket: return "Right Bracket";
		case KeyCode::Apostrophe: return "Apostrophe";
		case KeyCode::BackSpace: return "BackSpace";
		case KeyCode::Tab: return "Tab";
		case KeyCode::Enter: return "Enter";
		case KeyCode::Escape: return "Escape";
		case KeyCode::Space: return "Space";
		case KeyCode::CapsLock: return "CapsLock";
		case KeyCode::NumLock: return "NumLock";
		case KeyCode::Scroll: return "Scroll";
		case KeyCode::Pause: return "Pause";
		case KeyCode::Insert: return "Insert";
		case KeyCode::Delete: return "Delete";
		case KeyCode::Home: return "Home";
		case KeyCode::End: return "End";
		case KeyCode::PageUp: return "Page Up";
		case KeyCode::PageDown: return "Page Down";
		case KeyCode::LeftWin: return "Left Windows";
		case KeyCode::RightWin: return "Right Windows";
		default: return "Unknown";
		}
	}


	// MouseCodeからボタン名を取得
	const char* GetMouseButtonName(MouseCode code) {
		switch(code) {
		case MouseCode::Left: return "Left Button";
		case MouseCode::Right: return "Right Button";
		case MouseCode::Whell: return "Wheel Button";
		case MouseCode::Side1: return "Side Button 1";
		case MouseCode::Side2: return "Side Button 2";
		case MouseCode::Side3: return "Side Button 3";
		case MouseCode::Side4: return "Side Button 4";
		default: return "Unknown Button";
		}
	}


	//PadCodeからボタン名取得
	const char* GetPadButtonName(PadCode code) {
		switch(code) {
		case PadCode::Up: return "Up";
		case PadCode::Down: return "Down";
		case PadCode::Left: return "Left";
		case PadCode::Right: return "Right";
		case PadCode::Start: return "Start";
		case PadCode::Back: return "Back";
		case PadCode::LeftStick: return "Left Stick";
		case PadCode::RightStick: return "Right Stick";
		case PadCode::LeftShoulder: return "Left Shoulder";
		case PadCode::RightShoulder: return "Right Shoulder";
		case PadCode::A: return "A";
		case PadCode::B: return "B";
		case PadCode::X: return "X";
		case PadCode::Y: return "Y";
		default: return "Unknoun";
		}
	}

}



std::unique_ptr<Keyboard> Input::keyboard_ = nullptr;
std::unique_ptr<Mouse> Input::mouse_ = nullptr;
std::unique_ptr<Gamepad> Input::pad_ = nullptr;


/// ===================================================
/// インスタンス確保
/// ===================================================
Input* Input::GetInsatnce() {
	static Input instance;
	return &instance;
}



/// ===================================================
/// 初期化
/// ===================================================
void Input::Initialize(ONE::WinApp* winApp) {

	pWinApp_ = winApp;

	HRESULT hr = DirectInput8Create(
		winApp->GetWNDCLASS().hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)(&directInput_),
		nullptr
	);
	assert(SUCCEEDED(hr));



	keyboard_.reset(new Keyboard);
	keyboard_->Initialize(directInput_.Get(), winApp);

	mouse_.reset(new Mouse());
	mouse_->Initialize(directInput_.Get(), winApp);

	pad_.reset(new Gamepad());
	pad_->Initialize(directInput_.Get(), winApp);

}


/// ===================================================
/// 終了処理
/// ===================================================
void Input::Finalize() {
	keyboard_.reset();
	directInput_.Reset();
}


/// ===================================================
/// フレームの最初に行う処理
/// ===================================================
void Input::Update() {

	keyboard_->Begin();
	mouse_->Begin();
	pad_->Begin();


}


void Input::ImGuiDebug() {
	ImGui::Begin("Input");

	/// ---------------------------------------------------
	/// キーボードの入力チェック
	/// ---------------------------------------------------
	if(ImGui::TreeNodeEx("keyboard press checker", ImGuiTreeNodeFlags_DefaultOpen)) {

		for(uint32_t key = 0u; key < 256; ++key) {
			if(keyboard_->Press(KeyCode(key))) {
				ImGui::Text("%s is pressed", GetKeyName(KeyCode(key)));
			}
		}

		ImGui::TreePop();
	}


	/// ---------------------------------------------------
	/// マウスの入力チェック
	/// ---------------------------------------------------
	if(ImGui::TreeNodeEx("mouse press checker", ImGuiTreeNodeFlags_DefaultOpen)) {

		for(uint32_t button = 0u; button < uint32_t(MouseCode::Count); ++button) {
			if(PressMouse(MouseCode(button))) {
				ImGui::Text("%s is pressed", GetMouseButtonName(MouseCode(button)));
			}
		}

		ImGui::Separator();

		Vec2 position = MousePosition();
		Vec2 velocity = MouseVelocity();
		ImGui::Text("position : Vec2( %0.2f, %0.2f )", position.x, position.y);
		ImGui::Text("velocity : Vec2( %0.2f, %0.2f )", velocity.x, velocity.y);

		ImGui::Separator();

		ImGui::Text("scroll : float( %0.2f )", MouseScroll());

		ImGui::TreePop();
	}


	/// ---------------------------------------------------
	/// ゲームパッドの入力チェック
	/// ---------------------------------------------------
	if(ImGui::TreeNodeEx("gamepad press checker", ImGuiTreeNodeFlags_DefaultOpen)) {

		const std::array<PadCode, 14> padButtons = {
		PadCode::Up, PadCode::Down, PadCode::Left, PadCode::Right,
		PadCode::Start, PadCode::Back,
		PadCode::LeftStick, PadCode::RightStick,
		PadCode::LeftShoulder, PadCode::RightShoulder,
		PadCode::A, PadCode::B, PadCode::X, PadCode::Y
		};

		for(const PadCode& padButton : padButtons) {
			if(PressPadButton(PadCode(padButton))) {
				ImGui::Text("%s is pressed", GetPadButtonName(PadCode(padButton)));
			}
		}

		ImGui::Separator();

		Vec2 leftStick = GetLeftStick();
		Vec2 rigthStick = GetRightStick();
		ImGui::Text("LStick : Vec2( %f, %f )", leftStick.x, leftStick.y);
		ImGui::Text("RStick : Vec2( %f, %f )", rigthStick.x, rigthStick.y);

		uint8_t leftTrigger = GetLeftTrigger();
		uint8_t rightTrigger = GetRightTrigger();
		ImGui::Text("LTrigger : %d", static_cast<int>(leftTrigger));
		ImGui::Text("RTrigger : %d", static_cast<int>(rightTrigger));

		ImGui::TreePop();
	}

	ImGui::End();
}


/// ===================================================
/// キーボードのキー入力
/// ===================================================
bool Input::PressKey(KeyCode keycode) {
	return keyboard_->Press(keycode);
}

bool Input::TriggerKey(KeyCode keycode) {
	return keyboard_->Trigger(keycode);
}

bool Input::ReleaseKey(KeyCode keycode) {
	return keyboard_->Release(keycode);
}

bool Input::AnyKeyPress() {
	return keyboard_->AnyKeyPress();
}


/// ===================================================
/// マウス入力
/// ===================================================
bool Input::PressMouse(MouseCode mouseCode) {
	return mouse_->Press(mouseCode);
}

bool Input::TriggerMouse(MouseCode mouseCode) {
	return mouse_->Trigger(mouseCode);
}

bool Input::ReleaseMouse(MouseCode mouseCode) {
	return mouse_->Release(mouseCode);
}

Vec2 Input::MousePosition() {
	return mouse_->GetPosition();
}

Vec2 Input::MouseVelocity() {
	return mouse_->GetVelocity();
}

float Input::MouseScroll() {
	return mouse_->GetScroll();
}


/// ===================================================
/// ゲームパッド入力
/// ===================================================
bool Input::PressPadButton(PadCode padCode) {
	return pad_->Press(padCode);
}

bool Input::TriggerPadButton(PadCode padCode) {
	return pad_->Trigger(padCode);
}

bool Input::ReleasePadButton(PadCode padCode) {
	return pad_->Release(padCode);
}

bool Input::GamepadConnected(int gamepadIndex) {
	XINPUT_STATE state = {};
	// XInputGetStateで指定したコントローラーの状態を取得
	DWORD result = XInputGetState(gamepadIndex, &state);

	// 接続状態を確認
	return result == ERROR_SUCCESS;
}

bool Input::PadState(XINPUT_STATE& out) {
	return pad_->GetPadState(out);
}

bool Input::PadStatePrevious(XINPUT_STATE& out) {
	return pad_->GetPadStatePrevious(out);
}

bool Input::PadState(Pad pad) {
	return pad_->GetPadState(pad);
}

void Input::SetPadDeadZone(Pad pad, int32_t deadZoneL, int32_t deadZoneR) {
	pad_->SetPadDeadZone(pad, deadZoneL, deadZoneR);
}

Vector2 Input::GetLeftStick() {
	return pad_->GetLeftStick();
}

Vector2 Input::GetRightStick() {
	return pad_->GetRightStick();
}

Vector2 Input::GetLStick() {
	return pad_->GetLeftStick();
}

Vector2 Input::GetRStick() {
	return pad_->GetRightStick();
}

uint8_t Input::GetLeftTrigger() {
	return pad_->GetLeftTrigger();
}

uint8_t Input::GetRightTrigger() {
	return pad_->GetRightTrigger();
}

uint8_t Input::GetLTrigger() {
	return pad_->GetLeftTrigger();
}

uint8_t Input::GetRTrigger() {
	return pad_->GetRightTrigger();
}

bool Input::IsLStickStartMoving() {
	return pad_->IsLStickStartMoving();
}

bool Input::IsRStickStartMoving() {
	return pad_->IsRStickStartMoving();
}

Vec3 Input::MouseRay(float distance) {
	return mouse_->MouseRay(distance);
}

Vec3 Input::MouseNearPosition() {
	return mouse_->MouseNearPosition();
}

Vec3 Input::MouseFarPosition() {
	return mouse_->MouseFarPosition();
}
