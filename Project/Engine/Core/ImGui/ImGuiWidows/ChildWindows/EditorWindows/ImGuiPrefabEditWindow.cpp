#include "ImGuiPrefabEditWindow.h"

/// externals
#include <imgui.h>

ImGuiPrefabEditWindow::ImGuiPrefabEditWindow() {

}

void ImGuiPrefabEditWindow::ImGuiFunc() {
	if (!ImGui::Begin("Prefab Edit Window")) {
		ImGui::End();
		return;
	}

	

	ImGui::End();
}
