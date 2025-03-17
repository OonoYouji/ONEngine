#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>

void ImGuiHierarchyWindow::ImGuiFunc() {
	ImGui::Begin("Hierarchy");
	ImGui::Text("Hierarchy");

	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("%f, %f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	ImGui::Text("%f, %f", io.DisplaySize.x, io.DisplaySize.y);
	ImGui::End();
}
