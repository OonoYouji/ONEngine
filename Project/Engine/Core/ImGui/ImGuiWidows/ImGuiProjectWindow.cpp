#include "ImGuiProjectWindow.h"

/// stl
#include <filesystem>

/// external
#include <imgui.h>

ImGuiProjectWindow::ImGuiProjectWindow() {
	rootFolder_ = std::make_shared<Folder>();
	rootFolder_->name = "root";
	LoadFolder("./Assets", rootFolder_);
}

void ImGuiProjectWindow::ImGuiFunc() {
	ImGui::Begin("project", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// 2列のテーブルを作成
	if (ImGui::BeginTable("ProjectTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {
		// 左側（フォルダツリー）
		ImGui::TableSetupColumn("folders", ImGuiTableColumnFlags_WidthStretch, 0.3f); // 幅30%
		// 右側（ファイルリスト）
		ImGui::TableSetupColumn("files", ImGuiTableColumnFlags_WidthStretch, 0.7f); // 幅70%
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

	/// root folderを再帰的に描画
	DrawFolder(rootFolder_, 0);

	ImGui::EndChild();
}

void ImGuiProjectWindow::SelectFileView() {
	ImGui::BeginChild("SelectFileView", ImVec2(0, -FLT_MIN), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::EndChild();
}

void ImGuiProjectWindow::LoadProject(const std::string& _path) {
	LoadFolder(_path, rootFolder_);
}


void ImGuiProjectWindow::LoadFolder(const std::string& _path, std::shared_ptr<Folder> _folder) {
	for (const auto& entry : std::filesystem::directory_iterator(_path)) {
		if (entry.is_directory()) {

			auto subFolder = std::make_shared<Folder>();
			subFolder->name = entry.path().filename().string();
			_folder->folders.push_back(subFolder);
			LoadFolder(entry.path().string(), subFolder);

		} else if (entry.is_regular_file()) {

			File file;
			file.name = entry.path().filename().string();
			_folder->files.push_back(file);
		}
	}
}

void ImGuiProjectWindow::DrawFolder(std::shared_ptr<Folder> _folder, size_t _depth) {
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNodeEx(_folder->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		for (const auto& subFolder : _folder->folders) {
			DrawFolder(subFolder, _depth + 1);
		}
		for (const auto& file : _folder->files) {
			ImGui::BulletText("%s", file.name.c_str());
		}
		ImGui::TreePop();
	}
}
