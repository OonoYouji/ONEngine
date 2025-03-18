#include "ImGuiWindowCollection.h"

/// engine
#include "../ImGuiMainWindow.h"
#include "../ImGuiProjectWindow.h"
#include "../ImGuiGameWindow.h"
#include "../ImGuiSceneWindow.h"
#include "../ImGuiInspectorWindow.h"
#include "../ImGuiHierarchyWindow.h"


ImGuiWindowCollection::ImGuiWindowCollection(EntityCollection* _entityCollection) {
	entityCollection_ = _entityCollection;

	/// ここでwindowを生成する
	AddWindow(std::make_unique<ImGuiMainWindow>());
	AddWindow(std::make_unique<ImGuiProjectWindow>());
	AddWindow(std::make_unique<ImGuiGameWindow>());
	AddWindow(std::make_unique<ImGuiSceneWindow>());
	AddWindow(std::make_unique<ImGuiInspectorWindow>());
	AddWindow(std::make_unique<ImGuiHierarchyWindow>(entityCollection_));
}

ImGuiWindowCollection::~ImGuiWindowCollection() {}

void ImGuiWindowCollection::Update() {
	for (auto& window : iImguiWindows_) {
		window->ImGuiFunc();
	}
}
