#include <WinApp.h>


#include <windowsx.h>
#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_impl_dx12.h"
#include "Externals/imgui/imgui_impl_win32.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


/// ===================================================
/// 初期化
/// ===================================================
void ONE::WinApp::Initialize(const std::wstring& windowName) {

	CreateGameWindow(
		windowName.c_str(),
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
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	windowStyle_ = windowStyle;

	///- windowの設定
	wc_.lpfnWndProc = WindowProc;
	wc_.lpszClassName = title;
	wc_.hInstance = GetModuleHandle(nullptr);
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	ATOM atom = RegisterClass(&wc_);
	assert(atom != 0);


	///- 
	wrc_ = { 0, 0, sizeX, sizeY };
	BOOL result = AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);
	assert(result);

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

	assert(hwnd_ != nullptr);

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




