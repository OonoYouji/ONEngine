#include "ImGuiWindowCollection.h"

/// engine
#include "../ImGuiMainWindow.h"


ImGuiWindowCollection::ImGuiWindowCollection() {

	/// ここでwindowを生成する
	AddWindow(std::make_unique<ImGuiMainWindow>());

}
ImGuiWindowCollection::~ImGuiWindowCollection() {}

void ImGuiWindowCollection::Update() {
	for (auto& window : iImguiWindows_) {
		window->ImGuiFunc();
	}
}
