#include "ImGuiWindowCollection.h"

/// engine
#include "../ImGuiMainWindow.h"
#include "../GameWindows/ImGuiProjectWindow.h"
#include "../GameWindows/ImGuiGameSceneWindow.h"
#include "../GameWindows/ImGuiSceneWindow.h"
#include "../GameWindows/ImGuiInspectorWindow.h"
#include "../GameWindows/ImGuiHierarchyWindow.h"
#include "../GameWindows/ImGuiGameWindow.h"
#include "../ImGuiEditorWindow.h"
#include "../ImGuiEditorWindow.h"

ImGuiWindowCollection::ImGuiWindowCollection(EntityCollection* _entityCollection, GraphicsResourceCollection* _resourceCollection) {
	entityCollection_ = _entityCollection;
	resourceCollection_ = _resourceCollection;

	/// ここでwindowを生成する
	AddWindow(std::make_unique<ImGuiMainWindow>());

	AddWindow(std::make_unique<ImGuiGameWindow>());
	AddWindow(std::make_unique<ImGuiProjectWindow>());
	AddWindow(std::make_unique<ImGuiGameSceneWindow>());
	AddWindow(std::make_unique<ImGuiSceneWindow>(resourceCollection_));
	AddWindow(std::make_unique<ImGuiInspectorWindow>());
	AddWindow(std::make_unique<ImGuiHierarchyWindow>(entityCollection_));
	
	AddWindow(std::make_unique<ImGuiEditorWindow>());
}


ImGuiWindowCollection::~ImGuiWindowCollection() {}

void ImGuiWindowCollection::Update() {
	for (auto& window : iImguiWindows_) {
		window->ImGuiFunc();
	}
}
