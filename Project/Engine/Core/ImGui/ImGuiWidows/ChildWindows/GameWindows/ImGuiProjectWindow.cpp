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
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Core/ImGui/ImGuiSelection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"


namespace {

	/// @brief .slnファイルからの絶対パス
	const std::filesystem::path kRootPath = std::filesystem::absolute("./");


	/// @brief 指定した基準パスに対する、与えられた絶対パスの相対パスを計算して文字列で返す。
	/// @param absolutePath 相対パスに変換する絶対パス（std::filesystem::path の const 参照）。
	/// @param basePath 相対パスの基準となるパス（std::filesystem::path の const 参照）。
	/// @return 計算された相対パスを std::string として返す。
	std::string GetRelativePath(const std::filesystem::path& _absolutePath, const std::filesystem::path& _basePath = kRootPath) {
		std::filesystem::path relativePath = std::filesystem::relative(_absolutePath, _basePath);
		std::string relativeStr = relativePath.string();

		/// すでに "./" または "." で始まっていなければ付ける
		if (!relativeStr.empty() && relativeStr[0] != '.') {
			relativeStr = "./" + relativeStr;
		} else if (relativeStr == ".") {
			// 基準パスそのものの場合は "./" に統一
			relativeStr = "./";
		}

		return relativeStr;
	}


}	/// namespace



ImGuiProjectWindow::ImGuiProjectWindow(AssetCollection* _assetCollection, EditorManager* /*_editorManager*/)
	: pAssetCollection_(_assetCollection) {
	// 複数のルートディレクトリを設定
	rootPaths_ = {
		std::filesystem::absolute("./Assets"),
		std::filesystem::absolute("./Packages"),
		std::filesystem::absolute("../SubProjects/CSharpLibrary/Scripts")
	};

	currentPath_ = rootPaths_[0]; // 初期値として最初のルートディレクトリを設定

	// ファイル監視を開始
	std::vector<std::wstring> watchDirs;
	for (const auto& path : rootPaths_) {
		watchDirs.push_back(path.wstring());
	}
	fileWatcher_.Start(watchDirs);

	// 各ルートディレクトリのキャッシュを更新
	for (const auto& rootPath : rootPaths_) {
		UpdateDirectoryCache(rootPath);
		UpdateFileCache(rootPath);
	}
}

ImGuiProjectWindow::~ImGuiProjectWindow() {
	fileWatcher_.Stop();
}

void ImGuiProjectWindow::ShowImGui() {
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
        switch (event.action) {
        case FileEvent::Action::Added:
            eventType = "Added";
            HandleFileAdded(event.path);
            break;
        case FileEvent::Action::Removed:
            eventType = "Removed";
            HandleFileRemoved(event.path);
            break;
        case FileEvent::Action::Modified:
            eventType = "Modified";
            HandleFileModified(event.path);
            break;
        }
        ImGui::Text("%s: %s", eventType.c_str(), std::filesystem::path(event.path).filename().string().c_str());
    }

    /// ---------------------------------------------------
    /// テーブルレイアウトの開始
    /// ---------------------------------------------------
    if (ImGui::BeginTable("ProjectTable", 2, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Tree", ImGuiTableColumnFlags_WidthFixed, 250.0f);
        ImGui::TableSetupColumn("View", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();

        // --- 左カラム ---
        ImGui::TableSetColumnIndex(0);
        ImGui::BeginChild("TreeRegion");

        // rootPaths_ を最上位ノードとして表示
        for (const auto& rootPath : rootPaths_) {
            ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if (rootPath == currentPath_) {
                rootFlags |= ImGuiTreeNodeFlags_Selected;
            }

            bool rootNodeOpen = ImGui::TreeNodeEx(rootPath.filename().string().c_str(), rootFlags);
            if (ImGui::IsItemClicked()) {
                currentPath_ = rootPath;
            }

            if (rootNodeOpen) {
                DrawDirectoryTree(rootPath); // 子ディレクトリを描画
                ImGui::TreePop();
            }
        }

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

void ImGuiProjectWindow::DrawDirectoryTree(const std::filesystem::path& dir) {
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

void ImGuiProjectWindow::DrawFileView(const std::filesystem::path& dir) {
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
			key = GetRelativePath(key);
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

			AssetType type = GetAssetTypeFromExtension(extension);
			switch (type) {
			case AssetType::Texture:
			{
				/// previewが出来ないのでデフォルトのアイコンを表示
				if (extension != ".dds") {
					texture = pAssetCollection_->GetTexture(key);
				}
			}
			break;
			case AssetType::Audio:
				texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/mp3Icon.png");
				break;
			}

			if (!texture) {
				texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FileIcon.png");
			}

			ImGui::ImageButton(
				"##File",
				(ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr,
				ImMathf::CalculateAspectFitSize(texture->GetTextureSize(), 64.0f)
			);
		}


		/// ---------------------------------------------------
		/// ドラッグドロップの開始
		/// ---------------------------------------------------
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			static AssetPayload payload;
			payload.filePath = key;
			payload.guid = pAssetCollection_->GetAssetGuidFromPath(payload.filePath);

			AssetType type = pAssetCollection_->GetAssetTypeFromGuid(payload.guid);
			if (type == AssetType::Texture) {
				/// テクスチャのpreview
				Texture* tex = pAssetCollection_->GetTexture(payload.filePath);
				if (tex) {
					Vector2 aspectRatio = tex->GetTextureSize();
					aspectRatio /= (std::max)(aspectRatio.x, aspectRatio.y);
					ImTextureID texId = reinterpret_cast<ImTextureID>(tex->GetSRVGPUHandle().ptr);
					ImGui::Image(texId, ImVec2(64.0f * aspectRatio.x, 64.0f * aspectRatio.y));
				}
			}


			const AssetPayload* assetPtr = &payload;
			ImGui::SetDragDropPayload("AssetData", &assetPtr, sizeof(AssetPayload*));

			ImGui::EndDragDropSource();
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
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			PopupContextMenu(filePath);
		}

		ImGui::TextWrapped("%s", name.c_str());
		ImGui::EndGroup();
		ImGui::NextColumn();
		ImGui::PopID();
	}

	ImGui::Columns(1);
}

void ImGuiProjectWindow::PopupContextMenu(const std::filesystem::path& _dir) {

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

void ImGuiProjectWindow::SetRenameMode(const std::filesystem::path& _path) {
	isRenaming_ = true;
	renamingPath_ = _path;
	renameBuffer_ = _path.filename().string();
	justStartedRename_ = true;
	ImGui::SetKeyboardFocusHere(0);
}

void ImGuiProjectWindow::HandleFileAdded(const std::filesystem::path& _path) {
	if (std::filesystem::is_directory(_path)) {
		// ディレクトリが追加された場合、親ディレクトリのキャッシュを更新
		UpdateDirectoryCache(_path.parent_path());
		// 新しいディレクトリの開閉状態を初期化
		dirOpenState_[_path.string()] = false;
	} else {
		// ファイルが追加された場合、親ディレクトリのファイルキャッシュを更新
		UpdateFileCache(_path.parent_path());

		/// 新規アセットして登録
		std::string path = GetRelativePath(_path);
		Mathf::ReplaceAll(&path, "\\", "/");
		AssetType type = GetAssetTypeFromExtension(Mathf::FileExtension(path));
		if (type != AssetType::None) {
			pAssetCollection_->Load(path, type);
		}

	}
}

void ImGuiProjectWindow::HandleFileRemoved(const std::filesystem::path& _path) {
	if (std::filesystem::is_directory(_path)) {
		// ディレクトリが削除された場合、親ディレクトリのファイルキャッシュを更新
		UpdateDirectoryCache(_path.parent_path());
		// 削除されたディレクトリのキャッシュを削除
		directoryCache_.erase(_path.string());
		dirOpenState_.erase(_path.string());

	} else {
		// ファイルが削除された場合、親ディレクトリのファイルキャッシュを更新
		UpdateFileCache(_path.parent_path());
		// 削除されたファイルのキャッシュを削除
		fileCache_.erase(_path.string());

		/// アセットコレクションからも削除
		std::string path = GetRelativePath(_path);
		Mathf::ReplaceAll(&path, "\\", "/");
		pAssetCollection_->UnloadAssetByPath(path);
	}

	// 現在のパスが削除された場合、ルートパスに戻す
	if (_path == currentPath_) {
		currentPath_ = rootPath_;
	}
}

void ImGuiProjectWindow::HandleFileModified(const std::filesystem::path& _path) {
	if (!std::filesystem::exists(_path)) {
		return; // ファイルが存在しない場合は何もしない
	}

	/// パスのファイルを読み直す
	const std::string extension = _path.extension().string();
	AssetType type = GetAssetTypeFromExtension(extension);
	if (type != AssetType::None) {
		/// 絶対パスを相対パスに変換してから取得
		std::string path = GetRelativePath(_path);
		Mathf::ReplaceAll(&path, "\\", "/");
		pAssetCollection_->ReloadAsset(path);
	}

	// 親ディレクトリのキャッシュを更新
	UpdateFileCache(_path.parent_path());
}

void ImGuiProjectWindow::UpdateDirectoryCache(const std::filesystem::path& dir) {
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

		FileItem item;
		item.path = subdir;
		item.isDirectory = true;

		subdirectories.push_back(item);
		UpdateDirectoryCache(subdir); // 再帰
	}

	directoryCache_[dir.string()] = std::move(subdirectories);
}


void ImGuiProjectWindow::UpdateFileCache(const std::filesystem::path& dir) {
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

