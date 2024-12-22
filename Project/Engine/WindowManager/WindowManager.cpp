#include "WindowManager.h"

void WindowManager::Initialize() {}

void WindowManager::Finalize() {}



Window* WindowManager::GenerateWindow(const std::wstring& _windowName, const Vec2& _windowSize) {
	std::unique_ptr<Window> newWindow = std::make_unique<Window>();
	newWindow->Initialize(_windowName, _windowSize);

	windows_.push_back(std::move(newWindow));

	return newWindow.get();
}
