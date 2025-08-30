#include "ImGuiFileWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "../ChildWindows/FileWindows/ImGuiFile.h"

ImGuiFileWindow::ImGuiFileWindow() {

	/// windowの設定
	imGuiFlags_ |= ImGuiWindowFlags_NoMove;
	imGuiFlags_ |= ImGuiWindowFlags_NoResize;
	imGuiFlags_ |= ImGuiWindowFlags_NoTitleBar;
	imGuiFlags_ |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	/// 子ウィンドウの追加
	AddChild(std::make_unique<ImGuiFile>());

}

void ImGuiFileWindow::ShowImGui() {

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(EngineConfig::kWindowSize.x , EngineConfig::kWindowSize.y), ImGuiCond_Appearing);
	if (!ImGui::Begin("File", nullptr, imGuiFlags_)) {
		ImGui::End();
		return;
	}

	ImGuiID dockspaceID = ImGui::GetID("FileDockingSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f));

	UpdateChildren();

	ImGui::End();
}
