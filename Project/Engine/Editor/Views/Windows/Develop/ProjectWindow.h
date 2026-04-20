#pragma once

/// std
#include <string>
#include <filesystem>
#include <unordered_map>

/// engine
#include "../../EditorViewCollection.h"
#include "Engine/Editor/Math/FileWatcher/FileWatcher.h"
#include "Engine/Asset/Assets/Texture/Texture.h"


namespace ONEngine::Asset {
class AssetCollection;
}

namespace Editor {

class ProjectWindow : public IEditorWindow {
public:

	struct FileItem {
		std::filesystem::path path;
		bool isDirectory = false;
		std::string relativePath;
		ONEngine::Asset::Texture* displayTexture = nullptr;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ProjectWindow(ONEngine::Asset::AssetCollection* assetCollection);
	~ProjectWindow();

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

	/// @brief ImGui::Beginに用いるウィンドウ名を設定する
	/// @param _windowName ウィンドウ名
	void SetWindowName(const std::string& windowName);

	void DrawDirectoryTree(const std::filesystem::path& directory);
	void DrawFileView(const std::filesystem::path& directory);

	/// @brief 右クリックしたときに表示するポップアップメニュー
	/// @param directory 右クリックしたディレクトリのパス
	void PopupContextMenu(const std::filesystem::path& directory, std::filesystem::path& outDeletedPath);

	/// @brief DirectoryCacheの更新
	/// @param directory 更新対象のディレクトリパス
	void UpdateDirectoryCache(const std::filesystem::path& directory);

	/// @brief FileCacheの更新
	/// @param directory 更新対象のディレクトリパス
	void UpdateFileCache(const std::filesystem::path& directory);

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void DrawBreadcrumbs(const std::filesystem::path& directory, bool& outRequestChangeDir, std::filesystem::path& outNextTargetDir);
	void DrawFileList(const std::filesystem::path& directory, bool& outRequestChangeDir, std::filesystem::path& outNextTargetDir);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ONEngine::Asset::AssetCollection* pAssetCollection_;

	/// ImGui::Beginのlabelに使う
	std::string windowName_;

	std::vector<std::filesystem::path> rootPaths_;
	//std::filesystem::path rootPath_;
	std::filesystem::path currentPath_;   // 今見ているフォルダ
	std::unordered_map<std::string, bool> dirOpenState_; // ツリーの開閉状態
	std::filesystem::file_time_type lastWriteTime_;



	/// ----- フォルダツリー、ファイルの表示使う ----- ///
	/// ディレクトリツリーのキャッシュ
	std::unordered_map<std::string, std::vector<FileItem>> directoryCache_;
	/// ファイルリストのキャッシュ
	std::unordered_map<std::string, std::vector<FileItem>> fileCache_;


	FileWatcher fileWatcher_;

};

} /// Editor