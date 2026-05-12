#include "Window.h"

#pragma comment(lib, "winmm.lib")

// ImGui Win32 メッセージハンドラの宣言
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace {

static LRESULT MainWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
	if(ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return true;
	}

	switch(_msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_DESTROY: /// window破棄
		return 0;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

} /// namespace



namespace Engine::Core {


Window::Window() = default;
Window::~Window() = default;

void Window::Initialize(const std::wstring& windowName, const Engine::Math::Vector2Int& windowSize) {
	windowName_ = windowName;
	windowSize_ = windowSize;


	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	CreateGameWindow(windowName_.c_str(), WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME));
}

void Window::Shutdown() {
	CoUninitialize();
}

bool Window::CreateGameWindow(const wchar_t* title, UINT windowStyle) {

	timeBeginPeriod(1);

	windowClass_ = {};
	windowStyle_ = windowStyle;

	/// windowの設定
	windowClass_.lpfnWndProc = MainWindowProc;

	windowClass_.lpszClassName = title;
	windowClass_.hInstance = GetModuleHandle(nullptr);
	windowClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&windowClass_);

	wrc_ = { 0, 0, static_cast<int>(windowSize_.x), static_cast<int>(windowSize_.y) };
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

	hwnd_ = CreateWindowEx(
		0,
		windowClass_.lpszClassName,
		title,
		windowStyle_,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc_.right - wrc_.left,
		wrc_.bottom - wrc_.top,
		nullptr,
		nullptr,
		windowClass_.hInstance,
		nullptr
	);

	/// windowの生成できたかチェック
	if(!hwnd_) {
		//DWORD err = GetLastError();
		//Console::LogError("CreateWindowEx failed. Error code: " + std::to_string(err));
		//Assert(false, "Failed CreateWindowEx");
		return false;
	}


	/// window表示
	ShowWindow(hwnd_, SW_SHOW);

	return true;
}



void Window::Update() {
	while(PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
		if(msg_.message == WM_QUIT) {
			break;
		}

		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}

	/// 終了メッセージ
	if(msg_.message == WM_QUIT) {
		isProcessEnd_ = true;
		processMessage_ = true;
		return;
	}

	isProcessEnd_ = false;
	processMessage_ = false;
}


} /// namespace Engine::Core