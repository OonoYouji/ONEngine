#include "ImGuiGameWindow.h"

/// external
#include <imgui.h>

void ImGuiGameWindow::ImGuiFunc() {
	if (!ImGui::Begin("Game")) {
		ImGui::End();
		return;
	}

	// ドッキングスペースの作成
	ImGuiID dockspaceId = ImGui::GetID("game_dockspace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();
}