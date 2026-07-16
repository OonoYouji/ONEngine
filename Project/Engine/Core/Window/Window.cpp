#include "Window.h"

using namespace ONEngine;

/// windows
#include <windowsx.h>

/// engine
#include "WindowManager.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Input/Input.h"


/// pragma comment
#pragma comment(lib, "winmm.lib")


Window::Window() = default;

Window::~Window() {
	/// windowクラスの登録解除
	UnregisterClass(windowClass_.lpszClassName, windowClass_.hInstance);
	CloseWindow(hwnd_);
}


/**
 * @brief ウィンドウクラスの登録、ウィンドウの生成、およびスワップチェーンの初期化を行います。
 */
void Window::Initialize(const std::wstring& _windowName, const Vector2& _windowSize, DxManager* _dxm) {

	pDxManager_ = _dxm;

	windowName_ = _windowName;  /// 名前
	windowSize_ = _windowSize;  /// サイズ

	processMessage_ = false;

	dxSwapChain_ = std::make_unique<DxSwapChain>();
	dxSwapChain_->Initialize(_dxm, this);

	isFullScreen_ = false;
}

/**
 * @brief 描画前処理。レンダーターゲットビューの切り替えやバリア作成等を行います。
 */
void Window::PreDraw() {
	ID3D12GraphicsCommandList* commandList = pDxManager_->GetDxCommand()->GetCommandList();

	dxSwapChain_->BindViewportAndScissorRectForCommandList(commandList);
	dxSwapChain_->CreateBarrier(commandList, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	dxSwapChain_->ClearBackBuffer(commandList);
}

/**
 * @brief 描画後処理。レンダーターゲットをプレゼンテーション状態に戻すバリアを作成します。
 */
void Window::PostDraw() {
	ID3D12GraphicsCommandList* commandList = pDxManager_->GetDxCommand()->GetCommandList();
	dxSwapChain_->CreateBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

/**
 * @brief Alt+Enterキーなどによるウィンドウモード切り替えのキー判定を行います。
 */
void Window::Update() {
	if (Input::PressKey(DIK_RALT) || Input::PressKey(DIK_LALT)) {
		if (Input::TriggerKey(DIK_RETURN)) {
			ToggleFullScreen();
		}
	}
}

/**
 * @brief スワップチェーンのPresentを実行し、描画内容を画面に表示します。
 */
void Window::Present() {
	dxSwapChain_->Present();
}

/**
 * @brief ウィンドウが有効か判定します。
 */
bool Window::IsOpenWindow() {
	return hwnd_ != nullptr && IsWindow(hwnd_);
}

/**
 * @brief フルスクリーンとウィンドウモードの切り替えを行います。
 */
void Window::ToggleFullScreen() {
	/// ハイドかどうかを確認、ハイドならフルスクにしない
	if (!IsOpenWindow() || processMessage_) {
		return;
	}

	if (!isFullScreen_) {

		GetWindowRect(hwnd_, &wrc_);

		HMONITOR hMonitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfoW(hMonitor, &monitorInfo);

		/// 境界線なしスタイルに変更
		SetWindowLong(hwnd_, GWL_STYLE, WS_POPUP);
		SetWindowLong(hwnd_, GWL_EXSTYLE, 0);

		/// モニターサイズに合わせて最大化
		SetWindowPos(
			hwnd_, HWND_TOP,
			monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW
		);

		ShowWindow(hwnd_, SW_NORMAL);
		SetForegroundWindow(hwnd_);
		SetFocus(hwnd_);

	} else {
		// 通常ウィンドウに戻す
		SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);

		SetWindowPos(
			hwnd_, HWND_NOTOPMOST, 
			wrc_.left, wrc_.top,
			wrc_.right - wrc_.left, wrc_.bottom - wrc_.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE
		);

		ShowWindow(hwnd_, SW_NORMAL);
	}

	isFullScreen_ = !isFullScreen_;
}

/**
 * @brief ウィンドウハンドル（HWND）を取得します。
 */
HWND Window::GetHwnd() const {
	return hwnd_;
}

/**
 * @brief ウィンドウクラス（WNDCLASS）を取得します。
 */
const WNDCLASS& Window::GetWNDCLASS() const {
	return windowClass_;
}

/**
 * @brief 最新のプロセスメッセージIDを取得します。
 */
UINT Window::GetProcessMessage() const {
	return processMessage_;
}

/**
 * @brief ウィンドウの現在のクライアントサイズを取得します。
 */
const Vector2& Window::GetWindowSize() const {
	return windowSize_;
}
