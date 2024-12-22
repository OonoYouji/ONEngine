#include "Window.h"

/// windows
#include <windowsx.h>

/// engine
#include "WindowManager.h"

#pragma comment(lib, "winmm.lib")




Window::Window() {}

Window::~Window() {
	/// windowクラスの登録解除
	UnregisterClass(windowClass_.lpszClassName, windowClass_.hInstance);
	CloseWindow(hwnd_);
}


void Window::Initialize(const std::wstring& _windowName, const Vec2& _windowSize, UINT _windowStyle) {
	windowName_  = _windowName;  /// 名前
	windowSize_  = _windowSize;  /// サイズ
	windowStyle_ = _windowStyle; /// スタイル

	processMessage_ = false;
}

void Window::Update() {
	if(PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}

	///- 終了メッセージ
	if(msg_.message == WM_QUIT) {
		processMessage_ = true;
		return;
	}

	processMessage_ = false;
}

void Window::Draw() {}

