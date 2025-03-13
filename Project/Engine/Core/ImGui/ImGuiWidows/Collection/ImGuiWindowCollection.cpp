#include "ImGuiWindowCollection.h"

/// engine
#include "../ImGuiMainWindow.h"
#include "../ImGuiProjectWindow.h"


ImGuiWindowCollection::ImGuiWindowCollection() {

	/// ここでwindowを生成する
	AddWindow(std::make_unique<ImGuiMainWindow>());
	AddWindow(std::make_unique<ImGuiProjectWindow>());

}
ImGuiWindowCollection::~ImGuiWindowCollection() {}

void ImGuiWindowCollection::Update() {
	for (auto& window : iImguiWindows_) {
		window->ImGuiFunc();
	}
}
