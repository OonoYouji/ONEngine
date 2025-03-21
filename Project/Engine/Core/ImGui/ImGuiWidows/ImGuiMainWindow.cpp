#include "ImGuiMainWindow.h"

/// external
#include <imgui.h>


void ImGuiMainWindow::ImGuiFunc() {

	/// main menuの処理
	MainMenu();

	int imGuiFlags = 0;
	imGuiFlags |= ImGuiWindowFlags_NoMove;
	imGuiFlags |= ImGuiWindowFlags_NoResize;
	imGuiFlags |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(1280, 700), ImGuiCond_FirstUseEver);
	ImGui::Begin("main", nullptr, imGuiFlags);

	// ドッキングスペースの作成
	ImGuiID dockspaceID = ImGui::GetID("mainDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);

	//// ウィンドウを最背面に移動
	//ImGui::SetWindowPos("main", ImVec2(0, 20), ImGuiCond_Always);
	//ImGui::SetWindowSize("main", ImVec2(1280, 700), ImGuiCond_Always);
	//ImGui::SetWindowFocus(nullptr);

	ImGui::End();
}


void ImGuiMainWindow::MainMenu() {

	///!< 開いていない場合即returnする
	if (!ImGui::BeginMainMenuBar()) {
		return;
	}

	const char* debugModes[] = { "performance", "rendering", "object info", "effect", "terrain editor" };
	for (int i = 0; i < sizeof(debugModes) / sizeof(debugModes[0]); i++) {
		int save = currentMenuIndex_;

		if (i == save) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		}

		if (ImGui::Button(debugModes[i])) {
			currentMenuIndex_ = i;
		}

		if (i == save) {
			ImGui::PopStyleColor();
		}
	}

	ImGui::EndMainMenuBar();
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
