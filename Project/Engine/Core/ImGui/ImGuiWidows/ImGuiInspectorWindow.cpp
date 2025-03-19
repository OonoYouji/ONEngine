#include "ImGuiInspectorWindow.h"

/// external
#include <imgui.h>


void ImGuiInspectorWindow::ImGuiFunc() {
	if (!ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}