#pragma once

/// windows
#include <Windows.h>

/// std
#include <string>

/// lib
#include "Math/Vector2.h"



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

	void Initialize(const std::wstring& _windowName, const Vec2& _windowSize, UINT _windowStyle);
	void Update();
	void Draw();

private:

	/// ===================================================
	/// public : objects
	/// ===================================================

	std::wstring windowName_;
	Vec2         windowSize_;

	WNDCLASS     windowClass_;
	RECT         wrc_;
	HWND         hwnd_;
	MSG          msg_;
	UINT         windowStyle_;

	UINT         processMessage_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// windowのハンドルを取得
	/// </summary>
	/// <returns></returns>
	inline HWND GetHwnd() const { return hwnd_; }

	/// <summary>
	/// プロセスメッセージを取得
	/// </summary>
	/// <returns></returns>
	inline UINT GetProcessMessage() const { return processMessage_; }

private:
	Window(const Window&)             = delete;
	Window(Window&&)                  = delete;
	Window& operator= (const Window&) = delete;
	Window& operator= (Window&&)      = delete;
};

