#include "ImGuiMainWindow.h"

/// external
#include <imgui.h>


void ImGuiMainWindow::ImGuiFunc() {
	int imGuiFlags = 0;
	//imGuiFlags |= ImGuiWindowFlags_NoMove;
	//imGuiFlags |= ImGuiWindowFlags_NoResize;
	imGuiFlags |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags |= ImGuiWindowFlags_MenuBar;
	//ImGui::SetNextWindowPos(ImVec2(0, 0));
	//ImGui::SetNextWindowSize(ImVec2(1280, 720));
	ImGui::Begin("main", nullptr, imGuiFlags);

	// ドッキングスペースの作成
	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	//FileMenu();
	//EffectMenu();

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
