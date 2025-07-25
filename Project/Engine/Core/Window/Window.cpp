#include "Window.h"

/// windows
#include <windowsx.h>

/// engine
#include "WindowManager.h"
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Core/Utility/Input/Input.h"

#pragma comment(lib, "winmm.lib")




Window::Window() {}

Window::~Window() {
	/// windowクラスの登録解除
	UnregisterClass(windowClass_.lpszClassName, windowClass_.hInstance);
	CloseWindow(hwnd_);
}


void Window::Initialize(const std::wstring& _windowName, const Vec2& _windowSize, DxManager* _dxManager) {

	pDxManager_ = _dxManager;

	windowName_ = _windowName;  /// 名前
	windowSize_ = _windowSize;  /// サイズ

	processMessage_ = false;

	dxSwapChain_.reset(new DxSwapChain());
	dxSwapChain_->Initialize(_dxManager, this);

	isFullScreen_ = false;
}

void Window::PreDraw() {
	ID3D12GraphicsCommandList* commandList = pDxManager_->GetDxCommand()->GetCommandList();

	dxSwapChain_->BindViewportAndScissorRectForCommandList(commandList);
	dxSwapChain_->CreateBarrier(commandList, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	dxSwapChain_->ClearBackBuffer(commandList);
}

void Window::PostDraw() {
	ID3D12GraphicsCommandList* commandList = pDxManager_->GetDxCommand()->GetCommandList();
	dxSwapChain_->CreateBarrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void Window::Update() {

	if (Input::PressKey(DIK_RALT) || Input::PressKey(DIK_LALT)) {
		if (Input::TriggerKey(DIK_RETURN)) {
			ToggleFullScreen();
		}
	}

}

void Window::Present() {
	dxSwapChain_->Present();
}

bool Window::IsOpenWindow() {
	return hwnd_ != nullptr && IsWindow(hwnd_);
}

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

		//// 元の状態を覚えておく
		//GetWindowRect(hwnd_, &wrc_);

		//// 仮想フルスクリーン化
		//SetWindowLong(
		//	hwnd_, GWL_STYLE,
		//	windowStyle_ & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
		//);

		//HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
		//MONITORINFO info;
		//info.cbSize = sizeof(info);
		//GetMonitorInfo(monitor, &info);
		//fullscreenRect_.right = info.rcMonitor.right - info.rcMonitor.left;
		//fullscreenRect_.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

		//SetWindowPos(
		//	hwnd_, HWND_TOPMOST, 
		//	fullscreenRect_.left, fullscreenRect_.top,
		//	fullscreenRect_.right, fullscreenRect_.bottom, 
		//	SWP_FRAMECHANGED | SWP_NOACTIVATE
		//);
		//
		//ShowWindow(hwnd_, SW_MAXIMIZE);

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

