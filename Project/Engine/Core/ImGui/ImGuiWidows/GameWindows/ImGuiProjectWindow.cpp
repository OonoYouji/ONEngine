#include "ImGuiProjectWindow.h"

/// std
#include <filesystem>

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"

ImGuiProjectWindow::ImGuiProjectWindow(EditorManager* _editorManager)
	: pEditorManager_(_editorManager) {

	assetsRootFolder_ = std::make_shared<Folder>();
	assetsRootFolder_->name = "Assets";
	LoadFolder("./Assets", assetsRootFolder_);

	packagesRootFolder_ = std::make_shared<Folder>();
	packagesRootFolder_->name = "Packages";
	LoadFolder("./Packages", packagesRootFolder_);

	gameRootFolder_ = std::make_shared<Folder>();
	gameRootFolder_->name = "Game";
	LoadFolder("./Game", gameRootFolder_);

	selectedFolder_ = gameRootFolder_;
	isGameFolder_ = true;
}

void ImGuiProjectWindow::ImGuiFunc() {
	if (!ImGui::Begin("Project")) {
		ImGui::End();
		return;
	}

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
	DrawGameFolderHierarchy(gameRootFolder_, 0);
	DrawFolderHierarchy(assetsRootFolder_, 0);
	DrawFolderHierarchy(packagesRootFolder_, 0);

	ImGui::EndChild();
}

void ImGuiProjectWindow::SelectFileView() {
	ImGui::BeginChild("SelectFileView", ImVec2(0, -FLT_MIN), true, ImGuiWindowFlags_HorizontalScrollbar);
	DrawFolder(selectedFolder_);

	/// ドラッグ先の領域を設定
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos());
	ImGui::InvisibleButton("DropTargetArea", ImGui::GetWindowSize());

	/// ドロップされたファイルの処理
	/// Gameフォルダかチェック、違う場合は警告を出して無視
	if (ImGui::BeginDragDropTarget()) {
		ImGui::Text("Drop files here");

		if (isGameFolder_) {
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_HIERARCHY");
			if (payload) {
				IEntity* entity = static_cast<IEntity*>(payload->Data);
				pEditorManager_->ExecuteCommand<CreateNewEntityClassCommand>(entity, selectedFolder_->path);
			}
		} else {
			Console::Log("Cannot drop entity to this folder. Please select Game folder.");
		}


		ImGui::EndDragDropTarget();
	}





	ImGui::EndChild();
}

std::string ImGuiProjectWindow::NormalizePath(const std::string& _path) const {
	std::string result = _path;
	std::replace(result.begin(), result.end(), '\\', '/'); // WindowsのパスをUnixスタイルに変換
	return result;
}

void ImGuiProjectWindow::LoadFolder(const std::string& _path, std::shared_ptr<Folder> _folder) {

	_folder->path = _path;

	for (const auto& entry : std::filesystem::directory_iterator(_path)) {
		if (entry.is_directory()) {

			auto subFolder = std::make_shared<Folder>();
			subFolder->path = NormalizePath(entry.path().string());
			subFolder->name = NormalizePath(entry.path().filename().string());


			_folder->folders.push_back(subFolder);
			LoadFolder(subFolder->path, subFolder);

		} else if (entry.is_regular_file()) {

			File file;
			file.path = NormalizePath(entry.path().string());
			file.name = NormalizePath(entry.path().filename().string());
			_folder->files.push_back(file);
		}
	}
}

void ImGuiProjectWindow::DrawFolderHierarchy(std::shared_ptr<Folder> _folder, size_t _depth) {
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (selectedFolder_ == _folder) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	if (_folder->folders.empty()) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool nodeOpen = ImGui::TreeNodeEx(_folder->name.c_str(), nodeFlags);
	if (ImGui::IsItemClicked()) {
		selectedFolder_ = _folder;
		isGameFolder_ = false;
	}

	if (nodeOpen && !(_folder->folders.empty())) {
		for (const auto& subFolder : _folder->folders) {
			DrawFolderHierarchy(subFolder, _depth + 1);
		}
		ImGui::TreePop();
	}
}

void ImGuiProjectWindow::DrawGameFolderHierarchy(std::shared_ptr<Folder> _folder, size_t _depth) {
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (selectedFolder_ == _folder) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	if (_folder->folders.empty()) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	bool nodeOpen = ImGui::TreeNodeEx(_folder->name.c_str(), nodeFlags);
	if (ImGui::IsItemClicked()) {
		selectedFolder_ = _folder;
		isGameFolder_ = true;
	}

	if (nodeOpen && !(_folder->folders.empty())) {
		for (const auto& subFolder : _folder->folders) {
			DrawGameFolderHierarchy(subFolder, _depth + 1);
		}
		ImGui::TreePop();
	}
}

void ImGuiProjectWindow::DrawFolder(std::shared_ptr<Folder> _folder) {
	for (const auto& subFolder : _folder->folders) {
		uint32_t ptr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(subFolder.get()));

		/// ダブルクリックで選択
		ImGui::Selectable(subFolder->name.c_str(), ptr == selectedItemPtr_);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			selectedItemPtr_ = ptr;
			selectedFolder_ = subFolder;
		}
	}

	for (auto& file : _folder->files) {
		uint32_t ptr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&file));
		if (ImGui::Selectable(file.name.c_str(), ptr == selectedItemPtr_)) {
			selectedItemPtr_ = ptr;
			selectedFile_ = &file; // 選択されたファイルを保存
			/// TODO: inspectorに表示
		}


		/// ファイルを持ち始める
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			const char* cstr = file.path.c_str();
			ImGui::Text(cstr);
			ImGui::SetDragDropPayload("AssetData", cstr, strlen(cstr) + 1);

			ImGui::EndDragDropSource();
		}
	}



}
