#include <WinApp.h>


#include <windowsx.h>
#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


/// ===================================================
/// インスタンス確保
/// ===================================================
ONE::WinApp* ONE::WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}



/// ===================================================
/// 初期化
/// ===================================================
void ONE::WinApp::Initialize() {

	CreateGameWindow(
		L"2005_sin(ON);",
		WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
		kWindowSizeX, kWindowSizeY
	);

}



/// ===================================================
/// 終了処理
/// ===================================================
void ONE::WinApp::Finalize() {

	TerminateGameWindow();

}



/// ===================================================
/// 
/// ===================================================
UINT ONE::WinApp::ProcessMessage() {

	if(PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}

	///- 終了メッセージ
	if(msg_.message == WM_QUIT) {
		return true;
	}

	return false;
}

void ONE::WinApp::SetIsFullScreen(bool isFullScreen) {
	if(isFullScreen_ == isFullScreen) { return; }

	if(isFullScreen) {
		// 元の状態を覚えておく
		GetWindowRect(hwnd_, &wrc_);

		// 仮想フルスクリーン化
		SetWindowLong(
			hwnd_, GWL_STYLE,
			windowStyle_ &
			~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

		HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info;
		info.cbSize = sizeof(info);
		GetMonitorInfo(monitor, &info);
		fullscreenRect_.right = info.rcMonitor.right - info.rcMonitor.left;
		fullscreenRect_.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

		SetWindowPos(
			hwnd_, HWND_TOPMOST, fullscreenRect_.left, fullscreenRect_.top, fullscreenRect_.right,
			fullscreenRect_.bottom, SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hwnd_, SW_MAXIMIZE);

	} else {
		// 通常ウィンドウに戻す
		SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);

		SetWindowPos(
			hwnd_, HWND_NOTOPMOST, wrc_.left, wrc_.top,
			wrc_.right - wrc_.left, wrc_.bottom - wrc_.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hwnd_, SW_NORMAL);
	}

	isFullScreen_ = isFullScreen;
}



/// ===================================================
/// 
/// ===================================================
LRESULT ONE::WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if(ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	switch(msg) {
	case WM_DESTROY: ///- window破棄
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}



/// ===================================================
/// GameWindowの生成
/// ===================================================
void ONE::WinApp::CreateGameWindow(const wchar_t* title, UINT windowStyle, int sizeX, int sizeY) {

	///- COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	windowStyle_ = windowStyle;

	///- windowの設定
	wc_.lpfnWndProc = WindowProc;
	wc_.lpszClassName = L"ONEngine";
	wc_.hInstance = GetModuleHandle(nullptr);
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc_);


	///- 
	wrc_ = { 0, 0, sizeX, sizeY };
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

	///- 
	hwnd_ = CreateWindow(
		wc_.lpszClassName,
		title,
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



/// ===================================================
/// GameWindowの破棄
/// ===================================================
void ONE::WinApp::TerminateGameWindow() {

	///- windowクラスの登録解除
	UnregisterClass(wc_.lpszClassName, wc_.hInstance);

	///- COM終了
	CoUninitialize();

	CloseWindow(hwnd_);

}




