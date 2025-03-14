#include "ImGuiInspectorWindow.h"

/// external
#include <imgui.h>


void ImGuiInspectorWindow::ImGuiFunc() {
	ImGui::Begin("Inspector");
	ImGui::Text("Inspector");
	ImGui::End();
}