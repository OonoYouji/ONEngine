#include "ImGuiEditorWindow.h"

/// external
#include <imgui.h>

ImGuiEditorWindow::ImGuiEditorWindow() {}
ImGuiEditorWindow::~ImGuiEditorWindow() {}


void ImGuiEditorWindow::ImGuiFunc() {
	

	if (!ImGui::Begin("Editor")) {
		ImGui::End();
		return;
	}

	

	ImGui::End();
}
