#include "ImGuiGameSceneWindow.h"

/// external
#include <imgui.h>

void ImGuiGameSceneWindow::ImGuiFunc() {
	if (!ImGui::Begin("GameScene")) {
		ImGui::End();
		return;
	}


	ImGui::End();
}
