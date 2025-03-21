#include "ImGuiGameWindow.h"

/// external
#include <imgui.h>

void ImGuiGameWindow::ImGuiFunc() {
	if (!ImGui::Begin("Game")) {
		ImGui::End();
		return;
	}


	ImGui::End();
}