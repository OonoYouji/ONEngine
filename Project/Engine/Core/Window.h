#pragma once

#include <Windows.h>

/// std
#include <string>

/// engine::math
#include "Engine/Core/Math/Math.h"

namespace Engine::Core {

///
/// Windows OSでのウィンドウ表示クラス
///
class Window {
public:

	Window();
	~Window();

	void Initialize(const std::wstring& windowName, const Engine::Math::Vector2Int& windowSize);

	void Shutdown();

private:

	bool CreateGameWindow(const wchar_t* title, UINT windowStyle);

private:

	std::wstring windowName_;
	Engine::Math::Vector2Int windowSize_;

	WNDCLASS windowClass_;
	RECT wrc_;
	RECT fullscreenRect_;
	HWND hwnd_;
	MSG msg_;
	UINT windowStyle_;

};


} /// namespace Engine::Core