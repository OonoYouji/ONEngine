#include "WindowManager.h"

LRESULT WindowManager::MainWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	switch(_msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_DESTROY: /// window破棄
		return 0;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

LRESULT WindowManager::SubWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	switch(_msg) {
	case WM_CLOSE:
		DestroyWindow(_hwnd);
		return 0;
	case WM_DESTROY: /// window破棄
		return 0;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}



void WindowManager::Initialize() {
	/// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

}

void WindowManager::Finalize() {
	windows_.clear();
	/// COM終了
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}



Window* WindowManager::GenerateWindow(const std::wstring& _windowName, const Vec2& _windowSize, WindowType _windowType) {
	std::unique_ptr<Window> newWindow = std::make_unique<Window>();
	newWindow->Initialize(_windowName, _windowSize, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME));

	/// game windowを作成して表示する
	CreateGameWindow(_windowName.c_str(), _windowSize, newWindow.get(), _windowType);

	/// returnする用のpointer	
	Window* resultPtr = newWindow.get();

	windows_.push_back(std::move(newWindow));

	return resultPtr;
}

void WindowManager::CreateGameWindow(const wchar_t* _title, const Vec2& _size, Window* _windowPtr, WindowType _windowType) {

	timeBeginPeriod(1);


	/// windowの設定
	if(_windowType == WindowType::Main) {
		_windowPtr->windowClass_.lpfnWndProc   = MainWindowProc;
	} else {
		_windowPtr->windowClass_.lpfnWndProc   = SubWindowProc;
	}

	_windowPtr->windowClass_.lpszClassName = _title;
	_windowPtr->windowClass_.hInstance     = GetModuleHandle(nullptr);
	_windowPtr->windowClass_.hCursor       = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&_windowPtr->windowClass_);


	_windowPtr->wrc_ = { 0, 0, static_cast<int>(_size.x), static_cast<int>(_size.y) };
	AdjustWindowRect(&_windowPtr->wrc_, WS_OVERLAPPEDWINDOW, false);

	///- 
	_windowPtr->hwnd_ = CreateWindowEx(
		0,
		_windowPtr->windowClass_.lpszClassName,
		_title,
		_windowPtr->windowStyle_,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		_windowPtr->wrc_.right - _windowPtr->wrc_.left,
		_windowPtr->wrc_.bottom - _windowPtr->wrc_.top,
		nullptr,
		nullptr,
		_windowPtr->windowClass_.hInstance,
		nullptr
	);


	///- window表示
	ShowWindow(_windowPtr->hwnd_, SW_SHOW);
}
