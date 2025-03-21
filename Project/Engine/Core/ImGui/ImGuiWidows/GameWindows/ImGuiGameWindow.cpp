#include "ImGuiGameWindow.h"

/// external
#include <imgui.h>

/// engine
#include "../GameWindows/ImGuiProjectWindow.h"
#include "../GameWindows/ImGuiGameSceneWindow.h"
#include "../GameWindows/ImGuiSceneWindow.h"
#include "../GameWindows/ImGuiInspectorWindow.h"
#include "../GameWindows/ImGuiHierarchyWindow.h"

ImGuiGameWindow::ImGuiGameWindow(EntityCollection* _entityCollection, GraphicsResourceCollection* _resourceCollection) {

	AddChild(std::make_unique<ImGuiProjectWindow>());
	AddChild(std::make_unique<ImGuiGameSceneWindow>());
	AddChild(std::make_unique<ImGuiSceneWindow>(_resourceCollection));
	AddChild(std::make_unique<ImGuiInspectorWindow>());
	AddChild(std::make_unique<ImGuiHierarchyWindow>(_entityCollection));
}



void ImGuiGameWindow::ImGuiFunc() {
	if (!ImGui::Begin("Game")) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("GameDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateChildren();

	ImGui::End();
}