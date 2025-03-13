#include "ImGuiWindowCollection.h"

/// engine
#include "../ImGuiMainWindow.h"
#include "../ImGuiProjectWindow.h"
#include "../ImGuiGameWindow.h"
#include "../ImGuiSceneWindow.h"


ImGuiWindowCollection::ImGuiWindowCollection() {

	/// ここでwindowを生成する
	AddWindow(std::make_unique<ImGuiMainWindow>());
	AddWindow(std::make_unique<ImGuiProjectWindow>());
	AddWindow(std::make_unique<ImGuiGameWindow>());
	AddWindow(std::make_unique<ImGuiSceneWindow>());

}
ImGuiWindowCollection::~ImGuiWindowCollection() {}

void ImGuiWindowCollection::Update() {
	for (auto& window : iImguiWindows_) {
		window->ImGuiFunc();
	}
}
