#pragma once

/// windows
#include <Windows.h>

/// std
#include <string>
#include <memory>

/// engine
#include "Engine/Core/DirectX12/SwapChain/DxSwapChain.h"
#include "Engine/Core/Utility/Math/Vector2.h"



/// ///////////////////////////////////////////////////
/// windowクラス
/// ///////////////////////////////////////////////////
class Window {
	friend class WindowManager;
public:
	/// ===================================================
	/// public : method
	/// ===================================================

	Window();
	~Window();

	/// @brief 初期化
	/// @param _windowName Windowの名前
	/// @param _windowSize Windowのサイズ
	/// @param _dxm DxManagerのポインタ
	void Initialize(const std::wstring& _windowName, const Vector2& _windowSize, class DxManager* _dxm);

	/// @brief 描画前に行う処理
	void PreDraw();
	/// @brief 描画後に行う処理
	void PostDraw();

	/// @brief 更新
	void Update();

	/// @brief FrontBufferとBackBufferの交換
	void Present();

	/// @brief Windowが開いているか
	/// @return true:開いている false:閉じている
	bool IsOpenWindow();

	/// @brief フルスクリーンの切り替え
	void ToggleFullScreen();

private:
	/// ===================================================
	/// public : objects
	/// ===================================================

	/// ----- other class ----- ///
	class DxManager* pDxManager_;


	std::wstring                 windowName_;
	Vector2                      windowSize_;

	WNDCLASS                     windowClass_;
	RECT                         wrc_;
	RECT                         fullscreenRect_;
	HWND                         hwnd_;
	MSG                          msg_;
	UINT                         windowStyle_;

	UINT                         processMessage_;

	std::unique_ptr<DxSwapChain> dxSwapChain_;

	bool isFullScreen_ = false;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// @brief HWNDの取得
	HWND GetHwnd() const;

	/// @brief WNDCLASSの取得
	const WNDCLASS& GetWNDCLASS() const;

	/// @brief プロセスメッセージの取得
	UINT GetProcessMessage() const;

	/// @brief Windowサイズの取得
	const Vector2& GetWindowSize() const;


private:
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator= (const Window&) = delete;
	Window& operator= (Window&&) = delete;
};

