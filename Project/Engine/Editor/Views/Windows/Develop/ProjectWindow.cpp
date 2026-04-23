#include "ProjectWindow.h"

/// std
#include <filesystem>
#include <iostream>
#include <format>
#include <unordered_set>
#include <algorithm>

/// external
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

/// editor
#include "Engine/Editor/Manager/EditorManager.h"
#include "Engine/Editor/Math/AssetPayload.h"
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/ImGuiSelection.h"
#include "Engine/Asset/AssetType.h"

using namespace Editor;

namespace {

/// @brief .slnファイルからの絶対パス
const std::filesystem::path kRootPath = std::filesystem::absolute("./");


/// @brief 指定した基準パスに対する、与えられた絶対パスの相対パスを計算して文字列で返す。
std::string GetRelativePath(const std::filesystem::path& _absolutePath, const std::filesystem::path& _basePath = kRootPath) {
	std::filesystem::path relativePath = std::filesystem::relative(_absolutePath, _basePath);
	std::string relativeStr = relativePath.string();

	if(!relativeStr.empty() && relativeStr[0] != '.') {
		relativeStr = "./" + relativeStr;
	} else if(relativeStr == ".") {
		relativeStr = "./";
	}

	std::replace(relativeStr.begin(), relativeStr.end(), '\\', '/');

	return relativeStr;
}

}

ProjectWindow::ProjectWindow(ONEngine::Asset::AssetCollection* _assetCollection)
	: pAssetCollection_(_assetCollection) {
	windowName_ = "Project";

	rootPaths_ = { "./Assets", "./Packages", "../SubProjects/CSharpLibrary/Scripts" };
	currentPath_ = rootPaths_[0];

	// ファイル監視を開始
	std::vector<std::wstring> watchDirs;
	for(const auto& path : rootPaths_) {
		watchDirs.push_back(path.wstring());
	}
	fileWatcher_.Start(watchDirs);

	for(const auto& path : rootPaths_) {
		if(std::filesystem::exists(path)) {
			UpdateDirectoryCache(path);
		}
	}
	UpdateFileCache(currentPath_);
}

ProjectWindow::~ProjectWindow() {}


///	-------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// プロジェクトウィンドウの基礎的な処理
///	-------------------------------------------------------------------------------------------------------------------------------------------------------------------

///
/// プロジェクトウィンドウの表示
///
void ProjectWindow::ShowImGui() {
	if(ImGui::Begin(windowName_.c_str())) {
		ImGui::Columns(2);

		// 左側：フォルダツリー
		if(ImGui::BeginChild("DirectoryTree")) {
			for(const auto& root : rootPaths_) {
				if(std::filesystem::exists(root)) {
					DrawDirectoryTree(root);
				}
			}
		}
		ImGui::EndChild();

		ImGui::NextColumn();

		// 右側：ファイルビュー
		if(ImGui::BeginChild("FileView")) {
			DrawFileView(currentPath_);
		}
		ImGui::EndChild();

		ImGui::Columns(1);
	}
	ImGui::End();
}


///	-------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// 以下、処理ごとの関数実装
///	-------------------------------------------------------------------------------------------------------------------------------------------------------------------

///
/// プロジェクトウィンドウのウィンドウ名を設定
///
void ProjectWindow::SetWindowName(const std::string& windowName) {
	windowName_ = windowName;
}

///
/// プロジェクトのファイル構造をツリーで表示
///
void ProjectWindow::DrawDirectoryTree(const std::filesystem::path& directory) {
	std::string dirStr = directory.string();
	std::string dirName = directory.filename().string();

	// ルートパス自体（./Assets など）の名前が空（.）になる場合の対策
	if(dirName == "." || dirName == "") {
		dirName = directory.stem().string();
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if(currentPath_ == directory) flags |= ImGuiTreeNodeFlags_Selected;

	// 子ディレクトリがない場合は Leaf にする
	bool hasSubDirs = directoryCache_.contains(dirStr) && !directoryCache_[dirStr].empty();
	if(!hasSubDirs) flags |= ImGuiTreeNodeFlags_Leaf;

	bool isOpen = ImGui::TreeNodeEx(dirName.c_str(), flags);

	// クリックで中身を表示
	if(ImGui::IsItemClicked()) {
		currentPath_ = directory;
		UpdateFileCache(currentPath_);
	}

	if(isOpen) {
		if(directoryCache_.contains(dirStr)) {
			for(const auto& item : directoryCache_[dirStr]) {
				DrawDirectoryTree(item.path);
			}
		}
		ImGui::TreePop();
	}
}

///
/// ファイルのビュー表示
///
void ProjectWindow::DrawFileView(const std::filesystem::path& directory) {
	std::string dirStr = directory.string();
	if(!fileCache_.contains(dirStr)) {
		UpdateFileCache(directory);
	}

	bool requestChangeDir = false;
	std::filesystem::path nextTargetDir;

	DrawBreadcrumbs(directory, requestChangeDir, nextTargetDir);
	DrawFileList(directory, requestChangeDir, nextTargetDir);

	// ディレクトリ移動リクエストの処理（パンくずリスト・ダブルクリック共通）
	if(requestChangeDir) {
		currentPath_ = nextTargetDir;
		UpdateFileCache(currentPath_);
	}
}

///
/// パンくずリスト（階層ナビゲーション）の描画
///
void ProjectWindow::DrawBreadcrumbs(const std::filesystem::path& directory, bool& outRequestChangeDir, std::filesystem::path& outNextTargetDir) {
	// --- パンくずリスト（Breadcrumb）の描画 ---
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // ボタン背景を透明化
	std::filesystem::path cumulativePath;
	bool isFirst = true;

	for(const auto& part : directory) {
		if(!isFirst) {
			ImGui::SameLine(0, 4.0f);
			ImGui::Text("/");
			ImGui::SameLine(0, 4.0f);
		}

		if(cumulativePath.empty()) {
			cumulativePath = part;
		} else {
			cumulativePath /= part;
		}

		// パスの一部をボタンとして描画（クリックでその階層へ移動）
		if(ImGui::Button(part.string().c_str())) {
			outRequestChangeDir = true;
			outNextTargetDir = cumulativePath;
		}
		isFirst = false;
	}
	ImGui::PopStyleColor();
	ImGui::Separator();
	ImGui::Spacing();
	// ----------------------------------------
}

///
/// ファイル一覧の描画（ClipperとTableによる最適化版）
///
void ProjectWindow::DrawFileList(const std::filesystem::path& directory, bool& outRequestChangeDir, std::filesystem::path& outNextTargetDir) {
	std::string dirStr = directory.string();

	if(!fileCache_.contains(dirStr) || fileCache_[dirStr].empty()) return;

	auto& files = fileCache_[dirStr];

	// 削除予約用の変数
	std::filesystem::path pendingDeletePath;

	// --- ファイル一覧の描画 ---
	float iconSize = 64.0f;
	float padding = 16.0f;
	float cellSize = iconSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;

	// パネル幅から列数を計算
	int columnCount = static_cast<int>(panelWidth / cellSize);
	if(columnCount < 1) columnCount = 1;

	// 総行数を計算（切り上げ）
	int rowCount = (static_cast<int>(files.size()) + columnCount - 1) / columnCount;

	if(ImGui::BeginTable("FileGrid", columnCount)) {
		ImGuiListClipper clipper;
		clipper.Begin(rowCount);

		while(clipper.Step()) {
			for(int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
				ImGui::TableNextRow();

				for(int col = 0; col < columnCount; ++col) {
					size_t index = static_cast<size_t>(row * columnCount + col);
					if(index >= files.size()) break; // 最後の半端な列は抜ける

					ImGui::TableSetColumnIndex(col);

					auto& file = files[index];
					std::string name = file.path.filename().string();

					ImGui::PushID(static_cast<int>(index));

					ImGui::BeginGroup();

					// アイコン描画（キャッシュ済み）
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
					if(file.displayTexture) {
						ImGui::ImageButton("##Icon", (ImTextureID)(uintptr_t)file.displayTexture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
					} else {
						ImGui::Button("Icon", { iconSize, iconSize });
					}
					ImGui::PopStyleVar();

					// 名前描画
					ImGui::TextWrapped("%s", name.c_str());

					ImGui::EndGroup(); // グループ化ここまで

					// --- D&D処理 ---
					if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
						static AssetPayload payload;
						payload.filePath = file.relativePath; // キャッシュ済みの相対パスを利用
						payload.guid = pAssetCollection_->GetAssetGuidFromPath(payload.filePath);

						const AssetPayload* assetPtr = &payload;
						ImGui::SetDragDropPayload("AssetData", &assetPtr, sizeof(AssetPayload*));

						// ドラッグ中のプレビュー描画
						if(file.displayTexture) {
							ImGui::Image((ImTextureID)(uintptr_t)file.displayTexture->GetSRVGPUHandle().ptr, { 32.0f, 32.0f });
							ImGui::SameLine();
						}
						ImGui::Text("%s", name.c_str());
						ImGui::EndDragDropSource();
					}

					// --- クリック判定 ---
					if(ImGui::IsItemHovered()) {
						if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
							if(file.isDirectory) {
								outRequestChangeDir = true;
								outNextTargetDir = file.path;
							} else {
								const ONEngine::Guid& guid = pAssetCollection_->GetAssetGuidFromPath(file.relativePath);
								ImGuiSelection::SetSelectedObject(guid, SelectionType::Asset);
							}
						}
						if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
							ImGui::OpenPopup("FileContextMenu");
						}
					}

					// コンテキストメニューの呼び出し（削除予約パスを渡す）
					PopupContextMenu(file.path, pendingDeletePath);

					ImGui::PopID();
				}
			}
		}
		ImGui::EndTable();
	}

	// 描画ループを完全に抜けた後で、安全に削除処理とキャッシュ更新を行う
	if(!pendingDeletePath.empty()) {
		try {
			std::filesystem::remove_all(pendingDeletePath);

			// 元のファイルパスの末尾に ".meta" を付けたパスを作成
			std::filesystem::path metaPath = pendingDeletePath.string() + ".meta";
			if(std::filesystem::exists(metaPath)) {
				std::filesystem::remove(metaPath);
			}

			// 削除後、キャッシュを更新してUIに反映
			UpdateFileCache(directory);
			UpdateDirectoryCache(directory);
		} catch(const std::exception& e) {
			std::cerr << "Failed to delete file/folder: " << e.what() << std::endl;
		}
	}
}

///
/// ファイルを右クリックしたときの処理
///
void ProjectWindow::PopupContextMenu(const std::filesystem::path& filepath, std::filesystem::path& outDeletedPath) {
	if(ImGui::BeginPopup("FileContextMenu")) {
		if(ImGui::MenuItem("Reload")) {
			std::string path = GetRelativePath(filepath);
			pAssetCollection_->ReloadAsset(path);
		}

		// --- 削除機能の追加 ---
		ImGui::Separator();
		// 赤色で少し危険な操作であることをアピール
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
		if(ImGui::MenuItem("Delete")) {
			// ここでは削除せず、削除対象のパスを呼び出し元に伝えるだけ
			outDeletedPath = filepath;
		}
		ImGui::PopStyleColor();
		// ----------------------

		ImGui::EndPopup();
	}
}

///
/// ディレクトリのキャッシュを更新する
///
void ProjectWindow::UpdateDirectoryCache(const std::filesystem::path& directory) {
	if(!std::filesystem::exists(directory)) {
		directoryCache_.erase(directory.string());
		return;
	}

	std::vector<FileItem> subdirectories;
	try {
		for(const auto& entry : std::filesystem::directory_iterator(directory)) {
			if(!entry.is_directory()) continue;

			FileItem item;
			item.path = entry.path();
			item.isDirectory = true;
			subdirectories.push_back(item);

			UpdateDirectoryCache(entry.path());
		}
	} catch(...) {}

	directoryCache_[directory.string()] = std::move(subdirectories);
}

void ProjectWindow::UpdateFileCache(const std::filesystem::path& directory) {
	if(!std::filesystem::exists(directory)) {
		fileCache_.erase(directory.string());
		return;
	}

	std::vector<FileItem> files;
	try {
		for(const auto& entry : std::filesystem::directory_iterator(directory)) {
			if(entry.path().extension() == ".meta") continue;

			FileItem item;
			item.path = entry.path();
			item.isDirectory = entry.is_directory();
			item.relativePath = GetRelativePath(entry.path());

			// 拡張子を取得して小文字化
			std::string ext = item.path.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			// --- アイコン決定ロジック ---
			if(item.isDirectory) {
				item.displayTexture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FolderIcon.png");
				if(!item.displayTexture) item.displayTexture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FolderIcon.dds");
			} else if(ONEngine::Asset::CheckAssetType(ext, ONEngine::Asset::AssetType::Texture)) {
				item.displayTexture = pAssetCollection_->GetTexture(item.relativePath);
			} else if(ONEngine::Asset::CheckAssetType(ext, ONEngine::Asset::AssetType::Audio)) {
				item.displayTexture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/lets-icons-sound-none-256.png");
			} else if(ext == ".cs") {
				item.displayTexture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/ph-file-c-sharp-none-256.png");
			}

			// いずれにも当てはまらない、または読み込み失敗時のデフォルト
			if(!item.displayTexture) {
				item.displayTexture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FileIcon.png");
			}

			files.push_back(item);
		}
	} catch(...) {}

	fileCache_[directory.string()] = std::move(files);
}