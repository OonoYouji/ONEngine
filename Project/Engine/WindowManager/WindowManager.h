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

	WindowManager() = default;
	~WindowManager() = default;

	void Initialize();
	void Finalize();


	/// <summary>
	/// 新しいウィンドウを生成
	/// </summary>
	/// <param name="_windowName"> : ウィンドウの名前   </param>
	/// <param name="_windowSize"> : ウィンドウのサイズ </param>
	Window* GenerateWindow(const std::wstring& _windowName, const Vec2& _windowSize);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::vector<std::unique_ptr<Window>> windows_;

};

