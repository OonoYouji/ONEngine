#include "ImGuiEditorWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "../ChildWindows/EditorWindows/ImGuiPrefabViewWindow.h"
#include "../ChildWindows/EditorWindows/ImGuiPrefabInspectorWindow.h"
#include "../ChildWindows/EditorWindows/ImGuiPrefabFileWindow.h"

ImGuiEditorWindow::ImGuiEditorWindow(EntityComponentSystem* _ecs, GraphicsResourceCollection* _resourceCollection, EditorManager* _editorManager) {
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// 子windowの追加
	ImGuiPrefabInspectorWindow* inspector = static_cast<ImGuiPrefabInspectorWindow*>(
		AddChild(std::make_unique<ImGuiPrefabInspectorWindow>(_ecs, _editorManager)));

	AddChild(std::make_unique<ImGuiPrefabViewWindow>(_ecs, _resourceCollection));
	AddChild(std::make_unique<ImGuiPrefabFileWindow>(_ecs, _resourceCollection, inspector));
}


void ImGuiEditorWindow::ImGuiFunc() {
	
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(EngineConfig::kWindowSize.x, EngineConfig::kWindowSize.y), ImGuiCond_Appearing);
	if (!ImGui::Begin("Editor", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("EditorDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateChildren();

	ImGui::End();
}
