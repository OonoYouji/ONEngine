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

ProjectWindow::ProjectWindow(ONEngine::AssetCollection* _assetCollection, class EditorManager* _editorManager)
	: pAssetCollection_(_assetCollection) {
	windowName_ = "Project";

	// 【修正】表示したいルートフォルダを複数登録する
	rootPaths_ = { "./Assets", "./Packages", "../SubProjects/CSharpLibrary/Scripts" };

	// 初期表示は Assets にしておく
	currentPath_ = rootPaths_[0];

	// 【修正】すべてのルートディレクトリのキャッシュを初期化する
	for(const auto& path : rootPaths_) {
		if(std::filesystem::exists(path)) {
			UpdateDirectoryCache(path);
		}
	}
	UpdateFileCache(currentPath_);
}

ProjectWindow::~ProjectWindow() {}

void ProjectWindow::SetWindowName(const std::string& _windowName) {
	windowName_ = _windowName;
}

void ProjectWindow::ShowImGui() {
	if(ImGui::Begin(windowName_.c_str())) {
		ImGui::Columns(2);

		// 左側：フォルダツリー
		if(ImGui::BeginChild("DirectoryTree")) {
			// 【修正】登録されているすべてのルートを表示する
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

void ProjectWindow::DrawDirectoryTree(const std::filesystem::path& _dir) {
	std::string dirStr = _dir.string();
	std::string dirName = _dir.filename().string();

	// ルートパス自体（./Assets など）の名前が空（.）になる場合の対策
	if(dirName == "." || dirName == "") {
		dirName = _dir.stem().string();
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if(currentPath_ == _dir) flags |= ImGuiTreeNodeFlags_Selected;

	// 子ディレクトリがない場合は Leaf にする
	bool hasSubDirs = directoryCache_.contains(dirStr) && !directoryCache_[dirStr].empty();
	if(!hasSubDirs) flags |= ImGuiTreeNodeFlags_Leaf;

	bool isOpen = ImGui::TreeNodeEx(dirName.c_str(), flags);

	// クリックで中身を表示
	if(ImGui::IsItemClicked()) {
		currentPath_ = _dir;
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
void ProjectWindow::DrawFileView(const std::filesystem::path& _dir) {
	std::string dirStr = _dir.string();
	if(!fileCache_.contains(dirStr)) {
		UpdateFileCache(_dir);
	}

	float iconSize = 64.0f;
	float padding = 16.0f;
	float cellSize = iconSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if(columnCount < 1) columnCount = 1;

	ImGui::Columns(columnCount, nullptr, false);

	bool requestChangeDir = false;
	std::filesystem::path nextTargetDir;

	for(auto& file : fileCache_[dirStr]) {
		std::string name = file.path.filename().string();
		std::string key = GetRelativePath(file.path);

		ImGui::PushID(name.c_str());

		ImGui::BeginGroup();

		// --- アイコン描画 ---
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
		if(file.isDirectory) {
			ONEngine::Texture* texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FolderIcon.png");
			if(!texture) texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FolderIcon.dds");

			if(texture) {
				ImGui::ImageButton("##Folder", (ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
			} else {
				ImGui::Button("Folder", { iconSize, iconSize });
			}
		} else {
			ONEngine::Texture* texture = pAssetCollection_->GetTexture("./Packages/Textures/ImGui/FileIcons/FileIcon.png");
			if(texture) {
				ImGui::ImageButton("##File", (ImTextureID)(uintptr_t)texture->GetSRVGPUHandle().ptr, { iconSize, iconSize });
			} else {
				ImGui::Button("File", { iconSize, iconSize });
			}
		}
		ImGui::PopStyleVar();

		// --- 名前描画 ---
		ImGui::TextWrapped("%s", name.c_str());

		ImGui::EndGroup(); // グループ化ここまで

		// --- D&D処理 (元の正常な処理に復元 ＋ 判定位置の修正のみ) ---
		if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			// お客様の元の処理
			static AssetPayload payload;
			payload.filePath = key;
			payload.guid = pAssetCollection_->GetAssetGuidFromPath(payload.filePath);

			const AssetPayload* assetPtr = &payload;
			ImGui::SetDragDropPayload("AssetData", &assetPtr, sizeof(AssetPayload*));

			ImGui::Text("Dragging: %s", name.c_str());
			ImGui::EndDragDropSource();
		}

		// --- クリック判定 (EndGroupの後ろ) ---
		if(ImGui::IsItemHovered()) {
			if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if(file.isDirectory) {
					requestChangeDir = true;
					nextTargetDir = file.path;
				} else {
					const ONEngine::Guid& guid = pAssetCollection_->GetAssetGuidFromPath(key);
					ImGuiSelection::SetSelectedObject(guid, SelectionType::Asset);
				}
			}
			if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup("FileContextMenu");
			}
		}

		PopupContextMenu(file.path);

		ImGui::PopID();
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	if(requestChangeDir) {
		currentPath_ = nextTargetDir;
		UpdateFileCache(currentPath_);
	}
}

void ProjectWindow::PopupContextMenu(const std::filesystem::path& _path) {
	if(ImGui::BeginPopup("FileContextMenu")) {
		if(ImGui::MenuItem("Reload")) {
			std::string path = GetRelativePath(_path);
			pAssetCollection_->ReloadAsset(path);
		}
		ImGui::EndPopup();
	}
}

void ProjectWindow::UpdateDirectoryCache(const std::filesystem::path& _dir) {
	if(!std::filesystem::exists(_dir)) {
		directoryCache_.erase(_dir.string());
		return;
	}

	std::vector<FileItem> subdirectories;
	try {
		for(const auto& entry : std::filesystem::directory_iterator(_dir)) {
			if(!entry.is_directory()) continue;

			FileItem item;
			item.path = entry.path();
			item.isDirectory = true;
			subdirectories.push_back(item);

			UpdateDirectoryCache(entry.path());
		}
	} catch(...) {}

	directoryCache_[_dir.string()] = std::move(subdirectories);
}

void ProjectWindow::UpdateFileCache(const std::filesystem::path& _dir) {
	if(!std::filesystem::exists(_dir)) {
		fileCache_.erase(_dir.string());
		return;
	}

	std::vector<FileItem> files;
	try {
		for(const auto& entry : std::filesystem::directory_iterator(_dir)) {
			FileItem item;
			item.path = entry.path();
			item.isDirectory = entry.is_directory();
			files.push_back(item);
		}
	} catch(...) {}

	fileCache_[_dir.string()] = std::move(files);
}