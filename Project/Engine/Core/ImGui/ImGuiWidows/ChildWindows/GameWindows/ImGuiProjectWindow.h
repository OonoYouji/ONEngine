#pragma once

/// std
#include <string>
#include <filesystem>
#include <unordered_map>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"
#include "Engine/Core/ImGui/Math/FileWatcher/FileWatcher.h"


class ImGuiProjectWindow : public IImGuiChildWindow {
public:

	struct FileItem {
		std::filesystem::path path;
		bool isDirectory = false;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiProjectWindow(class AssetCollection* _assetCollection, class EditorManager* _editorManager);
	~ImGuiProjectWindow();

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

	/// @brief ImGui::Beginに用いるウィンドウ名を設定する
	/// @param _windowName ウィンドウ名
	void SetWindowName(const std::string& _windowName);

private:
	void DrawDirectoryTree(const std::filesystem::path& _dir);
	void DrawFileView(const std::filesystem::path& _dir);

	/// @brief 右クリックしたときに表示するポップアップメニュー
	/// @param _dir 右クリックしたディレクトリのパス
	void PopupContextMenu(const std::filesystem::path& _dir);


	void SetRenameMode(const std::filesystem::path& _path);


	/// @brief ファイルが追加された際の処理
	/// @param path 追加されたファイルのパス
	void HandleFileAdded(const std::filesystem::path& _path);

	/// @brief ファイルが削除された際の処理
	/// @param path 削除されたファイルのパス
	void HandleFileRemoved(const std::filesystem::path& _path);

	/// @brief ファイルが変更された際の処理
	/// @param path 変更されたファイルのパス
	void HandleFileModified(const std::filesystem::path& _path);

	void UpdateDirectoryCache(const std::filesystem::path& dir);

	void UpdateFileCache(const std::filesystem::path& _dir);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class AssetCollection* pAssetCollection_;

	/// ImGui::Beginのlabelに使う
	std::string windowName_;

	std::vector<std::filesystem::path> rootPaths_;
	std::filesystem::path rootPath_;
	std::filesystem::path currentPath_;   // 今見ているフォルダ
	std::unordered_map<std::string, bool> dirOpenState_; // ツリーの開閉状態
	std::filesystem::file_time_type lastWriteTime_;



	/// ----- フォルダツリー、ファイルの表示使う ----- ///
	/// ディレクトリツリーのキャッシュ
	std::unordered_map<std::string, std::vector<FileItem>> directoryCache_;
	/// ファイルリストのキャッシュ
	std::unordered_map<std::string, std::vector<FileItem>> fileCache_;


	/// ----- 名前の変更に使う ----- ///
	bool justStartedRename_;
	bool isRenaming_ = false;
	std::filesystem::path renamingPath_;
	std::string renameBuffer_;

	FileWatcher fileWatcher_;

};