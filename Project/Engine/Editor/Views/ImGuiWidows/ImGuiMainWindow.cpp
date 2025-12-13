#include "ImGuiMainWindow.h"


/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"

using namespace Editor;

void MainWindow::ShowImGui() {

	/// main menuの処理
	MainMenu();

	int imGuiFlags = 0;
	imGuiFlags |= ImGuiWindowFlags_NoMove;
	imGuiFlags |= ImGuiWindowFlags_NoResize;
	imGuiFlags |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_FirstUseEver);
	const ImVec2 windowSize = ImVec2(ONEngine::Vector2::HD.x, ONEngine::Vector2::HD.y);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
	ImGui::Begin("main", nullptr, imGuiFlags);

	// ドッキングスペースの作成
	ImGuiID dockspaceID = ImGui::GetID("mainDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);

	ImGui::End();
}


void MainWindow::MainMenu() {

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


