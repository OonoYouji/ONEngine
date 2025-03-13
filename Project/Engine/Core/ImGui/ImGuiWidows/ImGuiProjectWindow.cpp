#include "ImGuiProjectWindow.h"

/// external
#include <imgui.h>

void ImGuiProjectWindow::ImGuiFunc() {
	ImGui::Begin("project", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// 2列のテーブルを作成
	if (ImGui::BeginTable("ProjectTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {
		// 左側（フォルダツリー）
		ImGui::TableSetupColumn("Folders", ImGuiTableColumnFlags_WidthStretch, 0.3f); // 幅30%
		// 右側（ファイルリスト）
		ImGui::TableSetupColumn("Files", ImGuiTableColumnFlags_WidthStretch, 0.7f); // 幅70%
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();

		// --- 左側（フォルダツリー）---
		ImGui::TableSetColumnIndex(0);
		Hierarchy();

		// --- 右側（ファイルリスト）---
		ImGui::TableSetColumnIndex(1);
		SelectFileView();

		ImGui::EndTable();
	}


	ImGui::End();
}


void ImGuiProjectWindow::Hierarchy() {
	ImGui::BeginChild("Hierarchy", ImVec2(0, -FLT_MIN), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::EndChild();
}

void ImGuiProjectWindow::SelectFileView() {
	ImGui::BeginChild("SelectFileView", ImVec2(0, -FLT_MIN), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::EndChild();
}

