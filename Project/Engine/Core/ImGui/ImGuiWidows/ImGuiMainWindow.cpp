#include "ImGuiMainWindow.h"

/// external
#include <imgui.h>


void ImGuiMainWindow::ImGuiFunc() {
	ImGui::Begin("main", nullptr, ImGuiWindowFlags_MenuBar);

	FileMenu();
	EffectMenu();

	ImGui::End();
}


void ImGuiMainWindow::FileMenu() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("file")) {
			if (ImGui::MenuItem("new")) {
				/// new
			}
			if (ImGui::MenuItem("open")) {
				/// open
			}
			if (ImGui::MenuItem("save")) {
				/// save
			}
			if (ImGui::MenuItem("exit")) {
				/// exit
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void ImGuiMainWindow::EffectMenu() {

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("editor")) {

			/// オブジェクトの追加とか色々

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

}
