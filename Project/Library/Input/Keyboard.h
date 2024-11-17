#pragma once


//#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

#include <cmath>
#include <array>


namespace ONE { class WinApp; }


/// <summary>
/// キー入力に使う
/// </summary>
enum class KeyCode : BYTE {
	/// <summary>
	/// アルファベット
	/// </summary>
	A = DIK_A,	a = DIK_A,
	B = DIK_B,	b = DIK_B,
	C = DIK_C,	c = DIK_C,
	D = DIK_D,	d = DIK_D,
	E = DIK_E,	e = DIK_E,
	F = DIK_F,	f = DIK_F,
	G = DIK_G,	g = DIK_G,
	H = DIK_H,	h = DIK_H,
	I = DIK_I,	i = DIK_I,
	J = DIK_J,	j = DIK_J,
	K = DIK_K,	k = DIK_K,
	L = DIK_L,	l = DIK_L,
	M = DIK_M,	m = DIK_M,
	N = DIK_N,	n = DIK_N,
	O = DIK_O,	o = DIK_O,
	P = DIK_P,	p = DIK_P,
	Q = DIK_Q,	q = DIK_Q,
	R = DIK_R,	r = DIK_R,
	S = DIK_S,	s = DIK_S,
	T = DIK_T,	t = DIK_T,
	U = DIK_U,	u = DIK_U,
	V = DIK_V,	v = DIK_V,
	W = DIK_W,	w = DIK_W,
	X = DIK_X,	x = DIK_X,
	Y = DIK_Y,	y = DIK_Y,
	Z = DIK_Z,	z = DIK_Z,
	/// <summary>
	/// 数字
	/// </summary>
	Alpha0 = DIK_0,
	Alpha1 = DIK_1,
	Alpha2 = DIK_2,
	Alpha3 = DIK_3,
	Alpha4 = DIK_4,
	Alpha5 = DIK_5,
	Alpha6 = DIK_6,
	Alpha7 = DIK_7,
	Alpha8 = DIK_8,
	Alpha9 = DIK_9,
	/// <summary>
	/// テンキー
	/// </summary>
	Keypad0 = DIK_NUMPAD0,
	Keypad1 = DIK_NUMPAD1,
	Keypad2 = DIK_NUMPAD2,
	Keypad3 = DIK_NUMPAD3,
	Keypad4 = DIK_NUMPAD4,
	Keypad5 = DIK_NUMPAD5,
	Keypad6 = DIK_NUMPAD6,
	Keypad7 = DIK_NUMPAD7,
	Keypad8 = DIK_NUMPAD8,
	Keypad9 = DIK_NUMPAD9,
	KeypadEnter = DIK_NUMPADENTER,
	/// <summary>
	/// ファンクションキー
	/// </summary>
	F1 = DIK_F1,
	F2 = DIK_F2,
	F3 = DIK_F3,
	F4 = DIK_F4,
	F5 = DIK_F5,
	F6 = DIK_F6,
	F7 = DIK_F7,
	F8 = DIK_F8,
	F9 = DIK_F9,
	F10 = DIK_F10,
	F11 = DIK_F11,
	F12 = DIK_F12,
	/// <summary>
	/// 矢印キー
	/// </summary>
	UpArrow    = DIK_UP,         ArrowUp    = DIK_UP,
	DownArrow  = DIK_DOWN,       ArrowDown  = DIK_DOWN,
	LeftArrow  = DIK_LEFT,       ArrowLeft  = DIK_LEFT,
	RightArrow = DIK_RIGHT,      ArrowRight = DIK_RIGHT,
	/// <summary>
	/// 修飾キー
	/// </summary>
	LeftShift    = DIK_LSHIFT,   LShift   = DIK_LSHIFT,
	RightShift   = DIK_RSHIFT,   RShift   = DIK_RSHIFT,
	LeftControl  = DIK_LCONTROL, LControl = DIK_LCONTROL, 
	RightControl = DIK_RCONTROL, RControl = DIK_RCONTROL, 
	LeftAlt      = DIK_LALT,     LAlt     = DIK_LALT,
	RightAlt     = DIK_RALT,     RAlt     = DIK_RALT,
	/// <summary>
	/// 特殊キー
	/// </summary>
	Colon        = DIK_COLON,
	SemiColon    = DIK_SEMICOLON,
	Slash        = DIK_SLASH,
	BackSlash    = DIK_BACKSLASH,
	Comma        = DIK_COMMA,
	Period       = DIK_PERIOD,
	Minus        = DIK_MINUS,
	Equals       = DIK_EQUALS,
	Grave        = DIK_GRAVE,
	LeftBracket  = DIK_LBRACKET, LBracket = DIK_LBRACKET, 
	RightBracket = DIK_RBRACKET, RBracket = DIK_RBRACKET, 
	Apostrophe   = DIK_APOSTROPHE,
	/// <summary>
	/// その他
	/// </summary>
	BackSpace = DIK_BACKSPACE,
	Tab       = DIK_TAB,
	Enter     = DIK_RETURN,      Return = DIK_RETURN,
	Escape    = DIK_ESCAPE,
	Space     = DIK_SPACE,
	CapsLock  = DIK_CAPSLOCK,
	NumLock   = DIK_NUMLOCK,
	Scroll    = DIK_SCROLL,
	Pause     = DIK_PAUSE,
	Insert    = DIK_INSERT,
	Delete    = DIK_DELETE,
	Home      = DIK_HOME,
	End       = DIK_END,
	PageUp    = DIK_PGUP,
	PageDown  = DIK_PGDN,
	LeftWin   = DIK_LWIN,        LWin = DIK_LWIN, 
	RightWin  = DIK_RWIN,        RWin = DIK_RWIN, 
};



/// ===================================================
/// キーボード入力
/// ===================================================
class Keyboard final {
public:

	Keyboard();
	~Keyboard();

	void Initialize(IDirectInput8* directInput, ONE::WinApp* winApp);


	void Begin();

	bool Press(KeyCode keycode) const;
	bool Trigger(KeyCode keycode) const;
	bool Release(KeyCode keycode) const;

	bool AnyKeyPress() const;

	const std::array<BYTE, 256>& GetKeys()    const { return keys_; }
	const std::array<BYTE, 256>& GetPreKeys() const { return preKeys_; }

private:

	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

	std::array<BYTE, 256> keys_;	//- 今フレームの入力
	std::array<BYTE, 256> preKeys_; //- 前フレームの入力

	ONE::WinApp* pWinApp_ = nullptr;

private:
	Keyboard(const Keyboard&) = delete;
	Keyboard(Keyboard&&) = delete;
	Keyboard& operator= (const Keyboard&) = delete;
	Keyboard& operator= (Keyboard&&) = delete;
};