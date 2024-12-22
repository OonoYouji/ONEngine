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
public:
	
	/// ===================================================
	/// public : static method
	/// ===================================================

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


public:

	/// ===================================================
	/// public : method
	/// ===================================================

	Window();
	~Window();

	void Initialize(const std::wstring& _windowName, const Vec2& _windowSize);
	void Update();
	void Draw();


	/// <summary>
	/// windowを生成する
	/// </summary>
	/// <param name="_title"></param>
	/// <param name="_windowStyle"></param>
	/// <param name="_size"></param>
	void CreateGameWindow(const wchar_t* _title, UINT _windowStyle, const Vec2& _size);


private:

	/// ===================================================
	/// public : objects
	/// ===================================================

	std::wstring windowName_;
	Vec2        windowSize_;

	WNDCLASS    wc_;
	RECT        wrc_;
	HWND        hwnd_;
	MSG         msg_;
	UINT        windowStyle_;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// windowのハンドルを取得
	/// </summary>
	/// <returns></returns>
	inline HWND GetHwnd() const { return hwnd_; }



private:
	Window(const Window&)             = delete;
	Window(Window&&)                  = delete;
	Window& operator= (const Window&) = delete;
	Window& operator= (Window&&)      = delete;
};

