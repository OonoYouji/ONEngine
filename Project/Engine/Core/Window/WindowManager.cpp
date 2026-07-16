#include "WindowManager.h"

using namespace ONEngine;

/// external
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Tools/Assert.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);


static WindowManager* gWindowManager = nullptr;

/**
 * @brief シングルトンインスタンスを取得します。
 */
WindowManager* WindowManager::GetInstance() {
	return gWindowManager;
}

/**
 * @brief C#（Mono）側からウィンドウクライアントサイズを取得するためのバインド関数。
 */
void ONEngine::InternalGetWindowSize(Vector2* _size) {
	if(_size && gWindowManager && gWindowManager->GetMainWindow()) {
		*_size = gWindowManager->GetMainWindow()->GetWindowSize();
	}
}

/**
 * @brief メインウィンドウ用のウィンドウプロシージャ（メッセージハンドラ）。
 */
LRESULT WindowManager::MainWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
#ifdef DEBUG_MODE
	if (ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return true;
	}
#endif // DEBUG_MODE

	switch (_msg) {
	case WM_CLOSE:
		if (gWindowManager) {
			gWindowManager->SetCloseRequested(true);
		}
		return 0;
	case WM_DESTROY: /// window破棄
		return 0;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}

/**
 * @brief サブウィンドウ用のウィンドウプロシージャ（メッセージハンドラ）。
 */
LRESULT WindowManager::SubWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam) {
#ifdef DEBUG_MODE
	if (ImGui_ImplWin32_WndProcHandler(_hwnd, _msg, _wparam, _lparam)) {
		return true;
	}
#endif // DEBUG_MODE

	switch (_msg) {
	case WM_CLOSE:
	case WM_DESTROY: /// window破棄
		DestroyWindow(_hwnd);
		return 0;
	}

	return DefWindowProc(_hwnd, _msg, _wparam, _lparam);
}



WindowManager::WindowManager(DxManager* _dxm)
	: pDxManager_(_dxm) {}

WindowManager::~WindowManager() = default;


/**
 * @brief COMの初期化およびシングルトンインスタンスの設定を行います。
 */
void WindowManager::Initialize() {
	/// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	isProcessEnd_ = false;
	gWindowManager = this;
}

/**
 * @brief 全てのウィンドウ破棄およびCOMの解放を行います。
 */
void WindowManager::Finalize() {
	windows_.clear();
	/// COM終了
	CoUninitialize();
}

/**
 * @brief 全ウィンドウのウィンドウメッセージ処理およびライフサイクル更新を行います。
 */
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

		while (PeekMessage(&window->msg_, nullptr, 0, 0, PM_REMOVE)) {
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

/**
 * @brief メインウィンドウの描画前処理を行います。
 */
void WindowManager::MainWindowPreDraw() {
	GetMainWindow()->PreDraw();
}

/**
 * @brief メインウィンドウの描画後処理を行います。
 */
void WindowManager::MainWindowPostDraw() {
	GetMainWindow()->PostDraw();
}

/**
 * @brief 全ウィンドウに対する描画前処理を一括実行します。
 */
void WindowManager::PreDrawAll() {
	for (auto& window : windows_) {
		window->PreDraw();
	}
}

/**
 * @brief 全ウィンドウに対する描画後処理を一括実行します。
 */
void WindowManager::PostDrawAll() {
	for (auto& window : windows_) {
		window->PostDraw();
	}
}

/**
 * @brief 全ウィンドウのスワップチェーンプレゼンテーションを一括実行します。
 */
void WindowManager::PresentAll() {
	for (auto& window : windows_) {
		window->Present();
	}
}



/**
 * @brief 新しいウィンドウを生成して登録します。
 */
Window* WindowManager::GenerateWindow(const std::wstring& _windowName, const Vector2& _windowSize, WindowType _windowType, UINT _windowStyle) {
	std::unique_ptr<Window> newWindow = std::make_unique<Window>();

	/// game windowを作成して表示する
	CreateGameWindow(_windowName.c_str(), _windowSize, _windowStyle, newWindow.get(), _windowType);
	newWindow->Initialize(_windowName, _windowSize, pDxManager_);

	/// returnする用のpointer	
	Window* resultPtr = newWindow.get();

	windows_.push_back(std::move(newWindow));
	if (_windowType == WindowType::Main) {
		pMainWindow_ = resultPtr;
	}

	return resultPtr;
}

/**
 * @brief 実際にWin32ウィンドウを構築する内部関数です。
 */
void WindowManager::CreateGameWindow(const wchar_t* _title, const Vector2& _size, UINT _windowStyle, Window* _windowPtr, WindowType _windowType) {

	timeBeginPeriod(1);

	_windowPtr->windowClass_ = {};
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

	/// windowの生成できたかチェック
	if (!_windowPtr->hwnd_) {
		DWORD err = GetLastError();
		Console::LogError("CreateWindowEx failed. Error code: " + std::to_string(err));
		Assert(false, "Failed CreateWindowEx");
	}


	/// window表示
	ShowWindow(_windowPtr->hwnd_, SW_SHOW);
}

/**
 * @brief メインウィンドウ固有の毎フレーム更新処理を行います。
 */
void WindowManager::UpdateMainWindow() {
	pMainWindow_->Update();

	while (PeekMessage(&pMainWindow_->msg_, nullptr, 0, 0, PM_REMOVE)) {
		if (pMainWindow_->msg_.message == WM_QUIT) {
			break;
		}

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

/**
 * @brief メインウィンドウのオブジェクトポインタを取得します。
 */
Window* WindowManager::GetMainWindow() const {
	return pMainWindow_;
}

/**
 * @brief 現在フォアグラウンドまたはアクティブになっているウィンドウを取得します。
 */
Window* WindowManager::GetActiveWindow() const {

	HWND activeWindow = GetForegroundWindow();
	for (auto& window : windows_) {
		if (window->GetHwnd() == activeWindow) {
			return window.get();
		}
	}

	return GetMainWindow();
}

/**
 * @brief アプリケーションへの終了要求が発生しているか判定します。
 */
bool WindowManager::IsCloseRequested() const {
	return closeRequested_;
}

/**
 * @brief アプリケーションの終了要求フラグを明示的にセットします。
 */
void WindowManager::SetCloseRequested(bool _isCloseRequested) {
	closeRequested_ = _isCloseRequested;
}
