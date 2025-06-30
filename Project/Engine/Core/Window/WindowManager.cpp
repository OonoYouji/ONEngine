#include "WindowManager.h"

/// external
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);


LRESULT WindowManager::MainWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return true;
	}
#endif // _DEBUG

	switch (_msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_DESTROY: /// window破棄
		return 0;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

LRESULT WindowManager::SubWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return true;
	}
#endif // _DEBUG

	switch (_msg) {
	case WM_CLOSE:
	case WM_DESTROY: /// window破棄
		DestroyWindow(_hwnd);
		return 0;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}



WindowManager::WindowManager(DxManager* _dxManager)
	: dxManager_(_dxManager) {}

WindowManager::~WindowManager() {}


void WindowManager::Initialize() {
	/// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	isProcessEnd_ = false;
}

void WindowManager::Finalize() {
	windows_.clear();
	/// COM終了
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

void WindowManager::Update() {

	/// windowの更新
	for (auto itr = windows_.begin(); itr != windows_.end();) {
		if (!(*itr)->IsOpenWindow() && (*itr).get() != pMainWindow_) {
			itr = windows_.erase(itr);
		} else {
			++itr;
		}
	}

	/// main windowの更新
	UpdateMainWindow();

	/// sub windowの更新
	for (auto& window : windows_) {
		if (window.get() == pMainWindow_) {
			continue;
		}

		if (PeekMessage(&window->msg_, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&window->msg_);
			DispatchMessage(&window->msg_);
		}

		/// 終了メッセージ
		if (window->msg_.message == WM_QUIT) {
			window->processMessage_ = true;
			continue;
		}

		window->processMessage_ = false;
	}

}

void WindowManager::MainWindowPreDraw() {
	GetMainWindow()->PreDraw();
}

void WindowManager::MainWindowPostDraw() {
	GetMainWindow()->PostDraw();
}

void WindowManager::PreDrawAll() {
	for (auto& window : windows_) {
		window->PreDraw();
	}
}

void WindowManager::PostDrawAll() {
	for (auto& window : windows_) {
		window->PostDraw();
	}
}

void WindowManager::PresentAll() {
	for (auto& window : windows_) {
		window->Present();
	}
}



Window* WindowManager::GenerateWindow(const std::wstring& _windowName, const Vec2& _windowSize, WindowType _windowType) {
	std::unique_ptr<Window> newWindow = std::make_unique<Window>();

	/// game windowを作成して表示する
	CreateGameWindow(_windowName.c_str(), _windowSize, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), newWindow.get(), _windowType);
	newWindow->Initialize(_windowName, _windowSize, dxManager_);

	/// returnする用のpointer	
	Window* resultPtr = newWindow.get();

	windows_.push_back(std::move(newWindow));
	if (_windowType == WindowType::Main) {
		pMainWindow_ = resultPtr;
	}

	return resultPtr;
}

void WindowManager::CreateGameWindow(const wchar_t* _title, const Vec2& _size, UINT _windowStyle, Window* _windowPtr, WindowType _windowType) {

	timeBeginPeriod(1);

	_windowPtr->windowStyle_ = _windowStyle;

	/// windowの設定
	if (_windowType == WindowType::Main) {
		_windowPtr->windowClass_.lpfnWndProc = MainWindowProc;
	} else {
		_windowPtr->windowClass_.lpfnWndProc = SubWindowProc;
	}

	_windowPtr->windowClass_.lpszClassName = _title;
	_windowPtr->windowClass_.hInstance = GetModuleHandle(nullptr);
	_windowPtr->windowClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&_windowPtr->windowClass_);

	_windowPtr->wrc_ = { 0, 0, static_cast<int>(_size.x), static_cast<int>(_size.y) };
	AdjustWindowRect(&_windowPtr->wrc_, WS_OVERLAPPEDWINDOW, false);

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


	/// window表示
	ShowWindow(_windowPtr->hwnd_, SW_SHOW);
}

void WindowManager::UpdateMainWindow() {
	pMainWindow_->Update();

	if (PeekMessage(&pMainWindow_->msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&pMainWindow_->msg_);
		DispatchMessage(&pMainWindow_->msg_);
	}

	/// 終了メッセージ
	if (pMainWindow_->msg_.message == WM_QUIT) {
		isProcessEnd_ = true;
		pMainWindow_->processMessage_ = true;
		return;
	}

	isProcessEnd_ = false;
	pMainWindow_->processMessage_ = false;
}

Window* WindowManager::GetActiveWindow() const {

	HWND activeWindow = GetForegroundWindow();
	for (auto& window : windows_) {
		if (window->GetHwnd() == activeWindow) {
			return window.get();
		}
	}

	return GetMainWindow();
}
