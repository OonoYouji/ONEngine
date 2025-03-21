#include "ImGuiInspectorWindow.h"

/// external
#include <imgui.h>


void ImGuiInspectorWindow::ImGuiFunc() {
	if (!ImGui::Begin("Inspector")) {
		ImGui::End();
		return;
	}

	ImGui::End();
}