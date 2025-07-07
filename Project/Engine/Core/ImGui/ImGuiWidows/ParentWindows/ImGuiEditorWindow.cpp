#include "ImGuiEditorWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "../ChildWindows/EditorWindows/ImGuiPrefabEditWindow.h"

ImGuiEditorWindow::ImGuiEditorWindow() {
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;


	/// 子windowの追加
	AddChild(std::make_unique<ImGuiPrefabEditWindow>());
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
