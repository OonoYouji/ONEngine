#include "ImGuiGameWindow.h"

/// external
#include <imgui.h>

void ImGuiGameWindow::ImGuiFunc() {
	if (!ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoMove)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}
