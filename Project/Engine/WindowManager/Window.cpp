#include "Window.h"

/// windows
#include <windowsx.h>

/// engine
#include "WindowManager.h"
#include "Engine/DirectX12/Manager/DxManager.h"

#pragma comment(lib, "winmm.lib")




Window::Window() {}

Window::~Window() {
	/// windowクラスの登録解除
	UnregisterClass(windowClass_.lpszClassName, windowClass_.hInstance);
	CloseWindow(hwnd_);
}


void Window::Initialize(const std::wstring& _windowName, const Vec2& _windowSize, DxManager* _dxManager) {
	
	pDxManager_     = _dxManager;

	windowName_     = _windowName;  /// 名前
	windowSize_     = _windowSize;  /// サイズ

	processMessage_ = false;

	dxSwapChain_.reset(new DxSwapChain());
	dxSwapChain_->Initialize(_dxManager, this);
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

void Window::Present() {
	dxSwapChain_->Present();
}

