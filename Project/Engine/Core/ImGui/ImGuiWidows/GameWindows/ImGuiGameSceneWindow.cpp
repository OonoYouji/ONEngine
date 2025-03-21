#include "ImGuiGameSceneWindow.h"

/// external
#include <imgui.h>

void ImGuiGameSceneWindow::ImGuiFunc() {
	if (!ImGui::Begin("GameScene", nullptr, 0)) {
		ImGui::End();
		return;
	}


	ImGui::End();
}
