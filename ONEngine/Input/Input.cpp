#include <Input.h>

#include <cassert>
#include <WinApp.h>

#include <ImGuiManager.h>




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

}



std::unique_ptr<Keyboard> Input::keyboard_ = nullptr;


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
void Input::Begin() {

	keyboard_->Begin();

	ImGui::Begin("Input");

	for(uint32_t key = 0u; key < 256; ++key) {
		if(keyboard_->Push(KeyCode(key))) {
			ImGui::Text("%s is pressed", GetKeyName(KeyCode(key)));
		}
	}

	ImGui::End();
}


/// ===================================================
/// キーボードのキー入力
/// ===================================================
bool Input::PushKey(KeyCode keycode) {
	return keyboard_->Push(keycode);
}

bool Input::TriggerKey(KeyCode keycode) {
	return keyboard_->Trigger(keycode);
}

bool Input::ReleaseKey(KeyCode keycode) {
	return keyboard_->Release(keycode);
}
