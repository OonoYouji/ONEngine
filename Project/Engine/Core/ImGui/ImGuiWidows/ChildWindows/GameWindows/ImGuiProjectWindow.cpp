#include "ImGuiProjectWindow.h"

/// std
#include <filesystem>
#include <iostream>
#include <format>
#include <unordered_set>

/// external
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/ImGui/Math/AssetPayload.h"
#include "Engine/Core/ImGui/ImGuiSelection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"

ImGuiProjectExplorer::ImGuiProjectExplorer(AssetCollection* _assetCollection, EditorManager* /*_editorManager*/)
	: pAssetCollection_(_assetCollection) {
	rootPath_ = std::filesystem::absolute("./");
	currentPath_ = std::filesystem::absolute("./Assets");

	std::filesystem::path assetPath = std::filesystem::absolute("./Assets");
	std::filesystem::path packagesPath = std::filesystem::absolute("./Packages");

	/// ファイルの監視を開始
	fileWatcher_.Start({ assetPath, packagesPath });

	// rootPath_ の内容を directoryCache_ に追加
	UpdateDirectoryCache(rootPath_);

	// rootPath_ の内容を fileCache_ に追加
	UpdateFileCache(rootPath_);
}

ImGuiProjectExplorer::~ImGuiProjectExplorer() {
	fileWatcher_.Stop();
}

void ImGuiProjectExplorer::ShowImGui() {
	if (!ImGui::Begin("ImGuiProjectExplorer")) {
		ImGui::End();
		return;
	}


	/// ---------------------------------------------------
	/// ファイル監視イベントの処理
	/// ---------------------------------------------------
	auto events = fileWatcher_.ConsumeEvents();
	for (const auto& event : events) {
		std::string eventType;
		switch (event.type) {
		case FileEvent::Type::Added:
			eventType = "Added";
			HandleFileAdded(event.path); // ファイル追加処理
			break;
		case FileEvent::Type::Removed:
			eventType = "Removed";
			HandleFileRemoved(event.path); // ファイル削除処理
			break;
		case FileEvent::Type::Modified:
			eventType = "Modified";
			HandleFileModified(event.path); // ファイル変更処理
			break;
		}
		ImGui::Text("%s: %s", eventType.c_str(), std::filesystem::path(event.path).filename().string().c_str());
	}



	/// ---------------------------------------------------
	/// ProjectWindowの左上に出すMenuの内容
	/// ---------------------------------------------------

	if (ImGui::Button("Menu", ImVec2(120, 25))) {
		ImGui::OpenPopup("FileContextMenu");
	}

	ImGui::SameLine();

	ImGui::Text(": Search");
	ImGui::Separator();



	/// ---------------------------------------------------
	/// テーブルレイアウトの開始
	/// ---------------------------------------------------
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

void ImGuiProjectExplorer::DrawDirectoryTree(const std::filesystem::path& dir) {
	auto it = directoryCache_.find(dir.string());
	if (it == directoryCache_.end()) {
		return;
	}

	for (const auto& subDir : it->second) {
		const std::filesystem::path& subDirectoryPath = subDir.path;
		const std::string name = subDirectoryPath.filename().string();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (subDirectoryPath == currentPath_) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool nodeOpen = ImGui::TreeNodeEx(name.c_str(), flags);
		if (ImGui::IsItemClicked()) {
			currentPath_ = subDirectoryPath;
		}

		if (nodeOpen) {
			DrawDirectoryTree(subDirectoryPath);
			ImGui::TreePop();
		}
	}
}

void ImGuiProjectExplorer::DrawFileView(const std::filesystem::path& dir) {
	auto it = fileCache_.find(dir.string());
	if (it == fileCache_.end()) {
		return;
	}

	ImGui::Text("Current Path: %s", dir.string().c_str());
	ImGui::Separator();
	ImGui::Spacing();

	/// ----- ファイルのアイコンのサイズを決定 ----- ///
	float iconSize = 64.0f;
	int columnCount = (std::max)(1, (int)(ImGui::GetContentRegionAvail().x / (iconSize + 16.0f)));
	ImGui::Columns(columnCount, nullptr, false);

	for (const auto& file : it->second) {
		const std::filesystem::path& filePath = file.path;
		const std::string name = filePath.filename().string();

		/// engine内で扱うために相対パスに変換する
		std::string key = filePath.string();
		{
			/// filePathの絶対パスを相対パスに変換してから取得
			std::filesystem::path base = std::filesystem::absolute("./"); // 基準ディレクトリ
			std::filesystem::path relative = std::filesystem::relative(key, base);
			// 先頭が "." で始まらなければ "./" を付ける
			if (relative.empty() || relative.string()[0] != '.') {
				relative = std::filesystem::path(".") / relative;
			}

			key = relative.string();
			Mathf::ReplaceAll(&key, "\\", "/");
		}


		/// .meta ファイルは表示しない
		const std::string extension = filePath.extension().string();
		if (extension == ".meta") {
			continue;
		}


		ImGui::PushID(name.c_str());
		ImGui::BeginGroup();

		/// ----- アイコン表示 ----- ///
		if (file.isDirectory) {
			/// ----- ディレクトリなのでフォルダアイコンを表示 ----- ///
			Texture* texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FolderIcon.png");
			ImGui::ImageButton("##Folder", (ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
		} else {
			/// ----- ファイルなのでファイルアイコンを表示(拡張子ごとに) ----- ///
			Texture* texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FileIcon.png");

			AssetType type = GetAssetTypeFromExtension(filePath.extension().string());
			switch (type) {
			case AssetType::Texture:
			{
				texture = pAssetCollection_->GetTexture(key);
			}
			break;
			case AssetType::Audio:
				texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/mp3Icon.png");
				break;
			}

			if (!texture) {
				texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FileIcon.png");
			}

			ImGui::ImageButton("##File", (ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
		}


		/// ---------------------------------------------------
		/// ダブルクリックでフォルダを開く、ファイルを開く処理
		/// ---------------------------------------------------

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (file.isDirectory) {
				/// フォルダを開く
				currentPath_ = filePath;
			} else {
				/// インスペクターに表示する
				const Guid& guid = pAssetCollection_->GetAssetGuidFromPath(key);
				ImGuiSelection::SetSelectedObject(guid, SelectionType::Asset);
			}
		}

		/// ---------------------------------------------------
		/// 右クリックメニュー
		/// ---------------------------------------------------
		if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			PopupContextMenu(filePath);
		}



		ImGui::TextWrapped("%s", name.c_str());
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
				GenerateMaterialFile(_dir.string() + "/New Material.mat", nullptr);
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

void ImGuiProjectExplorer::HandleFileAdded(const std::filesystem::path& path) {
	if (std::filesystem::is_directory(path)) {
		// ディレクトリが追加された場合、親ディレクトリのキャッシュを更新
		UpdateDirectoryCache(path.parent_path());
		// 新しいディレクトリの開閉状態を初期化
		dirOpenState_[path.string()] = false;
	} else {
		// ファイルが追加された場合、親ディレクトリのファイルキャッシュを更新
		UpdateFileCache(path.parent_path());
	}
}

void ImGuiProjectExplorer::HandleFileRemoved(const std::filesystem::path& path) {
	if (std::filesystem::is_directory(path)) {
		// ディレクトリが削除された場合、親ディレクトリのキャッシュを更新
		UpdateDirectoryCache(path.parent_path());
		// 削除されたディレクトリのキャッシュを削除
		directoryCache_.erase(path.string());
		dirOpenState_.erase(path.string());
	} else {
		// ファイルが削除された場合、親ディレクトリのファイルキャッシュを更新
		UpdateFileCache(path.parent_path());
		// 削除されたファイルのキャッシュを削除
		fileCache_.erase(path.string());
	}

	// 現在のパスが削除された場合、ルートパスに戻す
	if (path == currentPath_) {
		currentPath_ = rootPath_;
	}
}

void ImGuiProjectExplorer::HandleFileModified(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		return; // ファイルが存在しない場合は何もしない
	}

	// 親ディレクトリのキャッシュを更新
	UpdateFileCache(path.parent_path());

	// 現在のパスが変更された場合、再描画をトリガー
	if (path.parent_path() == currentPath_) {
		// ImGuiの再描画はShowImGui内で行われるため、特別な処理は不要
		// キャッシュが更新されていれば、次回の描画で反映される
	}
}

void ImGuiProjectExplorer::UpdateDirectoryCache(const std::filesystem::path& dir) {
	// 無視するフォルダ名リスト
	static const std::unordered_set<std::wstring> ignoredDirs = {
		L".vs",
		L"Engine",
		L"Externals",
	};

	if (!std::filesystem::exists(dir)) {
		directoryCache_.erase(dir.string());
		return;
	}

	std::vector<FileItem> subdirectories;
	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		if (!entry.is_directory()) {
			continue;
		}

		const std::filesystem::path& subdir = entry.path();
		const std::wstring folderName = subdir.filename().wstring();

		/// 無視対象ならスキップ
		if (ignoredDirs.contains(folderName)) {
			continue;
		}


		FileItem item;
		item.path = subdir;
		item.isDirectory = true;

		subdirectories.push_back(item);
		UpdateDirectoryCache(subdir); // 再帰
	}

	directoryCache_[dir.string()] = std::move(subdirectories);
}


void ImGuiProjectExplorer::UpdateFileCache(const std::filesystem::path& dir) {
	if (!std::filesystem::exists(dir)) {
		fileCache_.erase(dir.string());
		return;
	}

	std::vector<FileItem> files;
	for (const auto& entry : std::filesystem::directory_iterator(dir)) {
		FileItem item;
		item.path = entry.path();

		if (entry.is_regular_file()) {
			item.isDirectory = false;
			files.push_back(item);
		} else if (entry.is_directory()) {
			// 再帰的にサブディレクトリを処理
			item.isDirectory = true;
			files.push_back(item);
			UpdateFileCache(entry.path());
		}
	}


	fileCache_[dir.string()] = std::move(files);
}

