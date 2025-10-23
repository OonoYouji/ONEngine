#include "ImGuiProjectWindow.h"

/// std
#include <filesystem>
#include <iostream>
#include <format>

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"
#include "Engine/Asset/Collection/AssetCollection.h"

ImGuiProjectWindow::ImGuiProjectWindow(AssetCollection* _grc, EditorManager* _editorManager)
	: pAssetCollection_(_grc), pEditorManager_(_editorManager) {

	// reloadといいつつも普通に読み込み
	ReloadProject();

	winName_ = "Project";
}


void ImGuiProjectWindow::ShowImGui() {
	if (!ImGui::Begin(winName_.c_str())) {
		ImGui::End();
		return;
	}

	if (ImGui::Button("ReloadProject")) {
		ReloadProject();
	}

	ImGui::SameLine();

	if (ImGui::Button("HotReload Resource")) {
		//pEditorManager_->ExecuteCommand<HotReloadResourceCommand>();
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

void ImGuiProjectWindow::SetWindowName(const std::string& _name) {
	winName_ = _name;
}

void ImGuiProjectWindow::ReloadProject() {
	assetsRootFolder_ = std::make_shared<Folder>();
	assetsRootFolder_->name = "Assets";
	LoadFolder("./Assets", assetsRootFolder_);

	packagesRootFolder_ = std::make_shared<Folder>();
	packagesRootFolder_->name = "Packages";
	LoadFolder("./Packages", packagesRootFolder_);

	scriptRootFolder_ = std::make_shared<Folder>();
	scriptRootFolder_->name = "Scripts";
	LoadFolder("../SubProjects/CSharpLibrary/Scripts/Game", scriptRootFolder_);

	selectedFolder_ = assetsRootFolder_;
	isGameFolder_ = true;
}


void ImGuiProjectWindow::Hierarchy() {
	ImGui::BeginChild("Project", ImVec2(0, -FLT_MIN), true, ImGuiWindowFlags_HorizontalScrollbar);

	/// root folderを再帰的に描画
	DrawFolderHierarchy(scriptRootFolder_, 0);
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
				GameEntity* entity = static_cast<GameEntity*>(payload->Data);
				pEditorManager_->ExecuteCommand<CreatePrefabCommand>(entity);
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

		/// 右クリックの動作
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup(subFolder->name.c_str());
		}

		/// ポップアップメニューの表示
		ShowContextMenu(subFolder->name, subFolder->path);
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


		/// ダブルクリックで選択、ファイルを開く
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			std::string folder = std::filesystem::absolute(file.path).string();
			ShellExecuteA(nullptr, "open", "explorer", folder.c_str(), nullptr, SW_SHOWNORMAL);
		}


		/// 右クリックの動作
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup(file.name.c_str());
		}

		{	/// ポップアップメニューの表示
			std::string folder = std::filesystem::absolute(file.path).string();
			/// folderからファイル名を除いたパスを取得
			folder = folder.substr(0, folder.find_last_of("/\\"));

			ShowContextMenu(file.name, folder);
		}
	}

}

void ImGuiProjectWindow::ShowContextMenu(const std::string& _contextMenuName, const std::string& _currentFolderName) {
	if (ImGui::BeginPopupContextWindow(_contextMenuName.c_str())) {

		/// Create系
		if (ImGui::BeginMenu("Create")) {

			if (ImGui::MenuItem("Folder")) {
				// 新規フォルダ作成の処理
			}

			ImGui::Separator();

			/// ゲームで使用するアセット系
			if (ImGui::MenuItem("C# Script")) {
				// C#スクリプト作成の処理
			}

			if (ImGui::MenuItem("Material")) {
				// マテリアル作成の処理
				GenerateMaterialFile(_currentFolderName + "/New Material.mat", nullptr);
			}


			ImGui::EndMenu();
		}



		if (ImGui::MenuItem("Show in Explorer")) {
			std::string folder = std::filesystem::absolute(_currentFolderName).parent_path().string();
			ShellExecuteA(nullptr, "open", "explorer", folder.c_str(), nullptr, SW_SHOWNORMAL);
		}

		ImGui::EndPopup();
	}
}










ImGuiProjectExplorer::ImGuiProjectExplorer(AssetCollection* _assetCollection, EditorManager* _editorManager)
	: pAssetCollection_(_assetCollection) {
	rootPath_ = "./";
	currentPath_ = rootPath_;
}

void ImGuiProjectExplorer::ShowImGui() {
	ImGui::Begin("ImGuiProjectExplorer");

	/// ProjectWindowの左上に出すMenuの内容
	if (ImGui::Button("Menu", ImVec2(120, 25))) {
		ImGui::OpenPopup("FileContextMenu");
	}

	ImGui::SameLine();

	ImGui::Text(": Search");
	

	ImGui::Separator();

	if (ImGui::BeginTable("ProjectTable", 2, ImGuiTableFlags_Resizable)) {
		// 左ペイン（ツリー）
		ImGui::TableSetupColumn("Tree", ImGuiTableColumnFlags_WidthFixed, 250.0f);
		ImGui::TableSetupColumn("View", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		// --- 左カラム ---
		ImGui::TableSetColumnIndex(0);
		ImGui::BeginChild("TreeRegion");
		DrawDirectoryTree(rootPath_);
		ImGui::EndChild();

		// --- 右カラム ---
		ImGui::TableSetColumnIndex(1);
		ImGui::BeginChild("ViewRegion");
		DrawFileView(currentPath_);
		ImGui::EndChild();

		ImGui::EndTable();
	}
	ImGui::End();
}

void ImGuiProjectExplorer::DrawDirectoryTree(const std::filesystem::path& _dir) {
	for (auto& entry : std::filesystem::directory_iterator(_dir)) {
		if (!entry.is_directory()) {
			continue;
		}

		const std::string name = entry.path().filename().string();
		//bool open = dirOpenState_[entry.path().string()];

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (entry.path() == currentPath_) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool nodeOpen = ImGui::TreeNodeEx(name.c_str(), flags);
		if (ImGui::IsItemClicked()) {
			currentPath_ = entry.path();
		}

		if (nodeOpen) {
			DrawDirectoryTree(entry.path());
			ImGui::TreePop();
		}
	}
}

void ImGuiProjectExplorer::DrawFileView(const std::filesystem::path& _dir) {
	if (!std::filesystem::exists(_dir)) {
		return;
	}


	/// 現在選択しているパスを段階的に表示
	ImGui::Text("Current Path: %s", _dir.string().c_str());


	ImGui::Separator();
	ImGui::Spacing();


	float iconSize = 64.0f;
	int columnCount = (std::max)(1, (int)(ImGui::GetContentRegionAvail().x / (iconSize + 16.0f)));
	ImGui::Columns(columnCount, nullptr, false);

	for (auto& entry : std::filesystem::directory_iterator(_dir)) {
		const auto& path = entry.path();
		std::string name = path.filename().string();
		std::string extension = path.extension().string();

		if (extension == ".meta") {
			continue; // .metaファイルは表示しない
		}


		ImGui::PushID(name.c_str());

		ImGui::BeginGroup();

		if (entry.is_directory()) {
			/// ----- フォルダアイコンを表示 ----- ///

			Texture* texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/Folder.png");
			ImGui::ImageButton("##Folder", (ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
		} else {
			/// ----- ファイルアイコンを表示 ----- ///

			/// extensionによってアイコンを変える
			std::string iconPath = "./Packages/Textures/ImGui/File.png"; // デフォルトアイコン
			if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
				iconPath = path.string();
				Mathf::ReplaceAll(&iconPath, "\\", "/"); // パスの区切り文字を変換
			} else if (extension == ".cs") {
				iconPath = "./Packages/Textures/ImGui/CSharpFile.png";
			} else if (extension == ".mat") {
				iconPath = "./Packages/Textures/ImGui/MaterialFile.png";
			} else if (extension == ".obj") {
				iconPath = "./Packages/Textures/ImGui/ModelFile.png";
			}

			Texture* texture = pAssetCollection_->GetTexture(iconPath);
			ImGui::ImageButton("##File", (ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
		}

		/// -------------------------------------------
		/// 右クリックメニュー
		/// -------------------------------------------
		PopupContextMenu(path);


		/// -------------------------------------------
		/// ダブルクリックでフォルダを開く、ファイルを選択
		/// -------------------------------------------
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (entry.is_directory()) {
				currentPath_ = path;
			} else {
				Console::Log(std::format("Selected file: {}", path.string()));
			}
		}


		/// -------------------------------------------
		/// 名前の変更処理をしているなら表示、そうでなければ通常表示
		/// -------------------------------------------
		if (isRenaming_ && renamingPath_ == path) {
			char buf[256];
			strcpy_s(buf, renameBuffer_.c_str());

			ImGui::PushItemWidth(iconSize + 20.0f);
			bool enterPressed = ImGui::InputText("##RenameInput", buf, IM_ARRAYSIZE(buf),
				ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

			if (justStartedRename_) {
				justStartedRename_ = false; // ← 1フレーム目はスキップ
			} else {
				if (!ImGui::IsItemActive() && !ImGui::IsItemFocused()) {
					isRenaming_ = false; // ← 2フレーム目以降のみチェック
				}
			}

			if (enterPressed) {
				std::string newName = buf;
				if (!newName.empty() && newName != name) {
					try {
						std::filesystem::rename(path, path.parent_path() / newName);
					} catch (const std::filesystem::filesystem_error& e) {
						Console::Log(std::format("Rename failed: {}", e.what()));
					}
				}
				isRenaming_ = false;
			}

			ImGui::PopItemWidth();
		} else {
			/// ----- 通常の名前表示 ----- ///
			ImGui::TextWrapped("%s", name.c_str());
		}



		ImGui::EndGroup();

		ImGui::NextColumn();

		ImGui::PopID();
	}

	ImGui::Columns(1);
}

void ImGuiProjectExplorer::PopupContextMenu(const std::filesystem::path& _dir) {

	/// 右クリックメニュー
	if (ImGui::BeginPopupContextItem("FileContextMenu")) {

		/// ---------------------------------------------------
		/// 新規ファイルの作成
		/// ---------------------------------------------------
		if (ImGui::BeginMenu("Create")) {

			if (ImGui::MenuItem("Folder")) {
				// 新規フォルダ名の決定（例: NewFolder, 重複チェック付き）
				std::filesystem::path newFolderPath;
				int counter = 1;
				do {
					newFolderPath = _dir / ("NewFolder" + std::to_string(counter));
					counter++;
				} while (std::filesystem::exists(newFolderPath));

				// フォルダを作成
				try {
					if (std::filesystem::create_directory(newFolderPath)) {
						// 作成成功
						std::cout << "Folder created: " << newFolderPath << std::endl;
					} else {
						// 作成失敗
						std::cerr << "Failed to create folder: " << newFolderPath << std::endl;
					}
				} catch (const std::filesystem::filesystem_error& e) {
					std::cerr << "Error creating folder: " << e.what() << std::endl;
				}
			}


			/// ---------------------------------------------------
			/// アセット系
			/// ---------------------------------------------------

			if (ImGui::MenuItem("C# Script")) {
				/// C#スクリプト作成の処理
			}

			if (ImGui::MenuItem("Material")) {
				/// マテリアル作成の処理
			}


			ImGui::EndMenu();
		}


		/// ---------------------------------------------------
		/// ファイルの操作
		/// ---------------------------------------------------

		if (ImGui::MenuItem("Show in Explorer")) {
			std::string folder = std::filesystem::absolute(_dir).string();
			ShellExecuteA(nullptr, "open", "explorer", folder.c_str(), nullptr, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("Open")) {
			std::string folder = std::filesystem::absolute(_dir).string();
			ShellExecuteA(nullptr, "open", "explorer", folder.c_str(), nullptr, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("Delete")) {
			/// 一度警告を出す
		}

		if (ImGui::MenuItem("Rename")) {
			SetRenameMode(_dir);
		}



		ImGui::EndPopup();
	}

}

void ImGuiProjectExplorer::SetRenameMode(const std::filesystem::path& _path) {
	isRenaming_ = true;
	renamingPath_ = _path;
	renameBuffer_ = _path.filename().string();
	justStartedRename_ = true;
	ImGui::SetKeyboardFocusHere(0);
}
