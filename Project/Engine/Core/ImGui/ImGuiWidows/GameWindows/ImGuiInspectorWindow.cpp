#include "ImGuiInspectorWindow.h"

/// external
#include <imgui.h>


void ImGuiInspectorWindow::ImGuiFunc() {
	if (!ImGui::Begin("Inspector", nullptr, 0)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}