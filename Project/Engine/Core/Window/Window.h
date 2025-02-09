#pragma once

/// windows
#include <Windows.h>

/// std
#include <string>
#include <memory>

/// engine
#include "Engine/Core/DirectX12/SwapChain/DxSwapChain.h"
#include "Engine/Core/Utility/Math/Vector2.h"



/// ===================================================
/// windowクラス
/// ===================================================
class Window {
	friend class WindowManager;
public:

	/// ===================================================
	/// public : method
	/// ===================================================

	Window();
	~Window();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="_windowName"> : ウィンドウの名前       </param>
	/// <param name="_windowSize"> : ウィンドウのサイズ     </param>
	/// <param name="_dxManager">  : DxManagerのポインター </param>
	void Initialize(const std::wstring& _windowName, const Vec2& _windowSize, class DxManager* _dxManager);

	/// <summary>
	/// 描画前の処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後の処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// front bufferとback bufferの交換
	/// </summary>
	void Present();

	/// <summary>
	/// windowを開いているか
	/// </summary>
	/// <returns></returns>
	bool IsOpenWindow();

private:

	/// ===================================================
	/// public : objects
	/// ===================================================

	std::wstring                 windowName_;
	Vec2                         windowSize_;

	WNDCLASS                     windowClass_;
	RECT                         wrc_;
	HWND                         hwnd_;
	MSG                          msg_;
	UINT                         windowStyle_;

	UINT                         processMessage_;

	std::unique_ptr<DxSwapChain> dxSwapChain_;
	class DxManager*             pDxManager_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// windowのハンドルを取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() const { return hwnd_; }

	/// <summary>
	/// プロセスメッセージを取得
	/// </summary>
	/// <returns></returns>
	UINT GetProcessMessage() const { return processMessage_; }

	/// <summary>
	/// ウィンドウのサイズを取得
	/// </summary>
	/// <returns></returns>
	const Vec2& GetWindowSize() const { return windowSize_; }

private:
	Window(const Window&)             = delete;
	Window(Window&&)                  = delete;
	Window& operator= (const Window&) = delete;
	Window& operator= (Window&&)      = delete;
};

