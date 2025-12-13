#include "ProjectWindow.h"

/// std
#include <filesystem>
#include <iostream>
#include <format>
#include <unordered_set>

/// external
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

/// editor
#include "Engine/Editor/Manager/EditorManager.h"
#include "Engine/Editor/Math/AssetPayload.h"
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/ImGuiSelection.h"


using namespace Editor;

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



ProjectWindow::ProjectWindow(ONEngine::AssetCollection* _assetCollection, EditorManager* /*_editorManager*/)
	: pAssetCollection_(_assetCollection) {

	/// デフォルトのウィンドウ名
	windowName_ = "ImGuiProjectExplorer";

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

ProjectWindow::~ProjectWindow() {
	fileWatcher_.Stop();
}

void ProjectWindow::ShowImGui() {
	if (!ImGui::Begin(windowName_.c_str())) {
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

		/// ---------------------------------------------------
		/// 左カラム : ディレクトリTree
		/// ---------------------------------------------------
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

		/// ---------------------------------------------------
		/// 右カラム : ファイルビュー
		/// ---------------------------------------------------
		ImGui::TableSetColumnIndex(1);
		ImGui::BeginChild("ViewRegion");
		DrawFileView(currentPath_);
		ImGui::EndChild();

		ImGui::EndTable();
	}
	ImGui::End();
}

void ProjectWindow::SetWindowName(const std::string& _windowName) {
	windowName_ = _windowName;
}

void ProjectWindow::DrawDirectoryTree(const std::filesystem::path& _dir) {
	/// ----- 引数のディレクトリを再帰的に表示していく ----- ///

	auto it = directoryCache_.find(_dir.string());
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

void ProjectWindow::DrawFileView(const std::filesystem::path& dir) {

	/// ========================================================================
	/// 1. 固定ヘッダー部分 (スクロールしない)
	/// ========================================================================

	// --- 現在のルート判定処理 (変更なし) ---
	const std::filesystem::path* pCurrentRoot = nullptr;
	std::filesystem::path relativePathFromRoot;
	for(const auto& root : rootPaths_) {
		try {
			std::filesystem::path rel = std::filesystem::relative(dir, root);
			if(!rel.empty() && rel.string().find("..") == std::string::npos) {
				pCurrentRoot = &root;
				relativePathFromRoot = rel;
				break;
			}
		} catch(...) { continue; }
	}

	if(!pCurrentRoot) {
		ImGui::Text("Path: %s", dir.string().c_str());
	} else {
		// --- 戻るボタン ---
		bool isRoot = std::filesystem::equivalent(dir, *pCurrentRoot);
		if(!isRoot) {
			if(ImGui::ArrowButton("##Back", ImGuiDir_Left)) {
				if(dir.has_parent_path()) {
					currentPath_ = dir.parent_path();
				}
			}
			ImGui::SameLine();
		} else {
			ImGui::Dummy(ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()));
			ImGui::SameLine();
		}

		// --- パンくずリスト ---
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.5f));

		std::string rootName = pCurrentRoot->filename().string();
		if(ImGui::Button(rootName.c_str())) {
			currentPath_ = *pCurrentRoot;
		}

		if(!isRoot && relativePathFromRoot != ".") {
			std::filesystem::path accumulatePath = *pCurrentRoot;
			for(const auto& part : relativePathFromRoot) {
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();
				accumulatePath /= part;
				if(ImGui::Button(part.string().c_str())) {
					currentPath_ = accumulatePath;
				}
			}
		}
		ImGui::PopStyleColor(3);
	}

	// ヘッダーとファイルリストの境界線
	ImGui::Separator();

	// ★ ここが重要：
	// ヘッダーの下に少し余白を入れてから、残りの領域すべてを使ってスクロールエリアを作る
	// ImVec2(0,0) を指定すると、「残りのスペース全て」を使う Child になります。
	if(ImGui::BeginChild("FileScrollingRegion", ImVec2(0, 0), false)) {

		/// ========================================================================
		/// 2. ファイルリスト部分 (ここだけスクロールする)
		/// ========================================================================

		ImGui::Spacing();

		auto it = fileCache_.find(dir.string());
		if(it != fileCache_.end()) {

			float iconSize = 64.0f;
			int columnCount = (std::max)(1, (int)(ImGui::GetContentRegionAvail().x / (iconSize + 16.0f)));
			ImGui::Columns(columnCount, nullptr, false);

			for(const auto& file : it->second) {
				const std::filesystem::path& filePath = file.path;
				const std::string name = filePath.filename().string();

				// パス変換
				std::string key = filePath.string();
				{
					key = GetRelativePath(key);
					ONEngine::FileSystem::ReplaceAll(&key, "\\", "/");
				}

				// .meta 除外
				const std::string extension = filePath.extension().string();
				if(extension == ".meta") {
					continue;
				}

				ImGui::PushID(name.c_str());
				ImGui::BeginGroup();

				// --- アイコン表示 ---
				if(file.isDirectory) {
					ONEngine::Texture* texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FolderIcon.png");
					ImGui::ImageButton("##Folder", (ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
				} else {
					ONEngine::Texture* texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FileIcon.png");
					ONEngine::AssetType type = ONEngine::GetAssetTypeFromExtension(extension);
					switch(type) {
					case ONEngine::AssetType::Texture:
						if(extension != ".dds") texture = pAssetCollection_->GetTexture(key);
						break;
					case ONEngine::AssetType::Audio:
						texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/mp3Icon.png");
						break;
					}
					if(!texture) texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FileIcon.png");

					ImGui::ImageButton(
						"##File",
						(ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr,
						ImMathf::CalculateAspectFitSize(texture->GetTextureSize(), 64.0f)
					);
				}

				// --- ドラッグ & ドロップ ---
				if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
					static AssetPayload payload;
					payload.filePath = key;
					payload.guid = pAssetCollection_->GetAssetGuidFromPath(payload.filePath);

					// プレビュー表示
					ONEngine::AssetType type = pAssetCollection_->GetAssetTypeFromGuid(payload.guid);
					if(type == ONEngine::AssetType::Texture) {
						ONEngine::Texture* tex = pAssetCollection_->GetTexture(payload.filePath);
						if(tex) {
							ONEngine::Vector2 aspectRatio = tex->GetTextureSize();
							aspectRatio /= (std::max)(aspectRatio.x, aspectRatio.y);
							ImTextureID texId = reinterpret_cast<ImTextureID>(tex->GetSRVGPUHandle().ptr);
							ImGui::Image(texId, ImVec2(64.0f * aspectRatio.x, 64.0f * aspectRatio.y));
						}
					}
					const AssetPayload* assetPtr = &payload;
					ImGui::SetDragDropPayload("AssetData", &assetPtr, sizeof(AssetPayload*));
					ImGui::EndDragDropSource();
				}

				// --- ダブルクリック処理 ---
				if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if(file.isDirectory) {
						currentPath_ = filePath;
						// Child, Group, ID, Columnのスタック整合性を保ってreturn
						ImGui::EndGroup();
						ImGui::PopID();
						ImGui::NextColumn(); // 次のカラムへ
						ImGui::Columns(1);   // カラムリセット
						ImGui::EndChild();   // Child終了
						return;
					} else {
						const ONEngine::Guid& guid = pAssetCollection_->GetAssetGuidFromPath(key);
						ImGuiSelection::SetSelectedObject(guid, SelectionType::Asset);
					}
				}

				// --- 右クリック ---
				if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("FileContextMenu");
				}
				PopupContextMenu(filePath);

				ImGui::TextWrapped("%s", name.c_str());
				ImGui::EndGroup();
				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Columns(1);
		}

		// Childウィンドウの終了
		ImGui::EndChild();
	}
}

void ProjectWindow::PopupContextMenu(const std::filesystem::path& _dir) {

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
				ONEngine::GenerateMaterialFile(_dir.string() + "/New Material.mat", nullptr);
			}


			ImGui::EndMenu();
		}


		/// ---------------------------------------------------
		/// ファイルの操作
		/// ---------------------------------------------------

		if (ImGui::MenuItem("Show in Explorer")) {
			std::string folder = std::filesystem::absolute(_dir).string();
			ShellExecuteA(nullptr, "open", "explorer.exe", folder.c_str(), nullptr, SW_SHOWNORMAL);
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

void ProjectWindow::SetRenameMode(const std::filesystem::path& _path) {
	isRenaming_ = true;
	renamingPath_ = _path;
	renameBuffer_ = _path.filename().string();
	justStartedRename_ = true;
	ImGui::SetKeyboardFocusHere(0);
}

void ProjectWindow::HandleFileAdded(const std::filesystem::path& _path) {
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
		ONEngine::FileSystem::ReplaceAll(&path, "\\", "/");
		ONEngine::AssetType type = ONEngine::GetAssetTypeFromExtension(ONEngine::FileSystem::FileExtension(path));
		if (type != ONEngine::AssetType::None) {
			pAssetCollection_->Load(path, type);
		}

	}
}

void ProjectWindow::HandleFileRemoved(const std::filesystem::path& _path) {
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
		ONEngine::FileSystem::ReplaceAll(&path, "\\", "/");
		pAssetCollection_->UnloadAssetByPath(path);
	}

	// 現在のパスが削除された場合、ルートパスに戻す
	if (_path == currentPath_) {
		currentPath_ = rootPath_;
	}
}

void ProjectWindow::HandleFileModified(const std::filesystem::path& _path) {
	if (!std::filesystem::exists(_path)) {
		return; // ファイルが存在しない場合は何もしない
	}

	/// パスのファイルを読み直す
	const std::string extension = _path.extension().string();
	ONEngine::AssetType type = ONEngine::GetAssetTypeFromExtension(extension);
	if (type != ONEngine::AssetType::None) {
		/// 絶対パスを相対パスに変換してから取得
		std::string path = GetRelativePath(_path);
		ONEngine::FileSystem::ReplaceAll(&path, "\\", "/");
		pAssetCollection_->ReloadAsset(path);
	}

	// 親ディレクトリのキャッシュを更新
	UpdateFileCache(_path.parent_path());
}

void ProjectWindow::UpdateDirectoryCache(const std::filesystem::path& _dir) {
	/// ----- 引数のディレクトリ内のサブディレクトリをキャッシュに保存 ----- ///

	if (!std::filesystem::exists(_dir)) {
		directoryCache_.erase(_dir.string());
		return;
	}

	std::vector<FileItem> subdirectories;
	for (const auto& entry : std::filesystem::directory_iterator(_dir)) {
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

	directoryCache_[_dir.string()] = std::move(subdirectories);
}


void ProjectWindow::UpdateFileCache(const std::filesystem::path& _dir) {
	/// ----- 引数のディレクトリ内のファイルをキャッシュに保存 ----- ///

	if (!std::filesystem::exists(_dir)) {
		fileCache_.erase(_dir.string());
		return;
	}

	std::vector<FileItem> files;
	for (const auto& entry : std::filesystem::directory_iterator(_dir)) {
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


	fileCache_[_dir.string()] = std::move(files);
}

