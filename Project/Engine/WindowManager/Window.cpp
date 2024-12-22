#include "Window.h"

#include <windowsx.h>

#pragma comment(lib, "winmm.lib")

LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	case WM_DESTROY: ///- window破棄
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}




Window::Window() {}

Window::~Window() {}

void Window::Initialize(const std::wstring& _windowName, const Vec2& _windowSize) {
	windowName_ = _windowName;
	windowSize_ = _windowSize;

	CreateGameWindow(windowName_.c_str(), WS_OVERLAPPEDWINDOW, windowSize_);

}

void Window::Update() {}

void Window::Draw() {}



void Window::CreateGameWindow(const wchar_t* _title, UINT _windowStyle, const Vec2& _size) {


	timeBeginPeriod(1);

	///- COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	windowStyle_      = _windowStyle;

	///- windowの設定
	wc_.lpfnWndProc   = WindowProc;
	wc_.lpszClassName = _title;
	wc_.hInstance     = GetModuleHandle(nullptr);
	wc_.hCursor       = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc_);


	///- 
	wrc_ = { 0, 0, static_cast<int>(_size.x), static_cast<int>(_size.y) };
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

	///- 
	hwnd_ = CreateWindowEx(
		0,
		wc_.lpszClassName,
		_title,
		windowStyle_,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc_.right - wrc_.left,
		wrc_.bottom - wrc_.top,
		nullptr,
		nullptr,
		wc_.hInstance,
		nullptr
	);


	///- window表示
	ShowWindow(hwnd_, SW_SHOW);

}
