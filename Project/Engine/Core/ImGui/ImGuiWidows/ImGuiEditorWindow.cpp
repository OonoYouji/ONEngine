#include "ImGuiEditorWindow.h"

/// external
#include <imgui.h>

ImGuiEditorWindow::ImGuiEditorWindow() {
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;
}


void ImGuiEditorWindow::ImGuiFunc() {
	
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(1280, 700), ImGuiCond_Appearing);
	if (!ImGui::Begin("Editor", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("EditorDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateChildren();

	ImGui::End();
}
