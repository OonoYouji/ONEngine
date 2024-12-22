#pragma once

/// std
#include <memory>
#include <vector>

/// engine
#include "Window.h"

/// ===================================================
/// windowの管理クラス
/// ===================================================
class WindowManager final {
public:

	/// ===================================================
	/// public : enum
	/// ===================================================

	enum class WindowType {
		Main,
		Sub,
	};


public:

	/// ===================================================
	/// public : static method
	/// ===================================================

	/// <summary>
	/// mainのwindowプロシージャ
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	static LRESULT CALLBACK MainWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);

	/// <summary>
	/// subのwindowプロシージャ
	/// </summary>
	/// <param name="_hwnd"></param>
	/// <param name="_msg"></param>
	/// <param name="_wparam"></param>
	/// <param name="_lparam"></param>
	/// <returns></returns>
	static LRESULT CALLBACK SubWindowProc(HWND _hwnd, UINT _msg, WPARAM _wparam, LPARAM _lparam);


public:

	/// ===================================================
	/// public : method
	/// ===================================================

	WindowManager(class DxManager* _dxManager);
	~WindowManager();

	void Initialize();
	void Finalize();


	/// <summary>
	/// 新しいウィンドウを生成
	/// </summary>
	/// <param name="_windowName"> : ウィンドウの名前   </param>
	/// <param name="_windowSize"> : ウィンドウのサイズ </param>
	Window* GenerateWindow(const std::wstring& _windowName, const Vec2& _windowSize, WindowType _windowType = WindowType::Sub);


	/// <summary>
	/// game windowを生成
	/// </summary>
	void CreateGameWindow(const wchar_t* _title, const Vec2& _size, UINT _windowStyle, Window* _windowPtr, WindowType _windowType);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<Window>> windows_;

	class DxManager*                     pDxManager_ = nullptr;

};

