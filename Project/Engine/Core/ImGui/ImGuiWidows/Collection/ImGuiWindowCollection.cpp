#include "ImGuiWindowCollection.h"

/// engine
#include "../ImGuiMainWindow.h"
#include "../ImGuiProjectWindow.h"
#include "../ImGuiGameWindow.h"
#include "../ImGuiSceneWindow.h"
#include "../ImGuiInspectorWindow.h"
#include "../ImGuiHierarchyWindow.h"


ImGuiWindowCollection::ImGuiWindowCollection(EntityCollection* _entityCollection, GraphicsResourceCollection* _resourceCollection) {
	entityCollection_ = _entityCollection;
	resourceCollection_ = _resourceCollection;

	/// ここでwindowを生成する
	AddWindow(std::make_unique<ImGuiMainWindow>());
	AddWindow(std::make_unique<ImGuiProjectWindow>());
	AddWindow(std::make_unique<ImGuiGameWindow>());
	AddWindow(std::make_unique<ImGuiSceneWindow>(resourceCollection_));
	AddWindow(std::make_unique<ImGuiInspectorWindow>());
	AddWindow(std::make_unique<ImGuiHierarchyWindow>(entityCollection_));
}


ImGuiWindowCollection::~ImGuiWindowCollection() {}

void ImGuiWindowCollection::Update() {
	for (auto& window : iImguiWindows_) {
		window->ImGuiFunc();
	}
}
