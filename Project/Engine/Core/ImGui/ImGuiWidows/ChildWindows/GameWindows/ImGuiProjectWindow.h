#pragma once

/// std
#include <string>
#include <filesystem>
#include <unordered_map>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiProjectWindow
/// ///////////////////////////////////////////////////
class ImGuiProjectWindow : public IImGuiChildWindow {
	/// ===================================================
	/// private : sub class
	/// ===================================================

	struct File {
		std::string path;
		std::string name;
	};

	struct Folder {
		std::string path; ///< フォルダのパス
		std::string name;
		std::vector<std::shared_ptr<Folder>> folders;
		std::vector<File> files;
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiProjectWindow(class AssetCollection* _assetCollection, class EditorManager* _editorManager);
	~ImGuiProjectWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

	/// @brief ウィンドウ名の設定
	void SetWindowName(const std::string& _name);

	/// @brief Projectの再読み込み
	void ReloadProject();

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
	
	/// @brief プロジェクトの階層表示
	void Hierarchy();

	/// @brief プロジェクトのファイル表示
	void SelectFileView();

	/// @brief WindowsのパスをUnixスタイルに変換する
	/// @param _path 変換するパス
	/// @return 変換後のパス
	std::string NormalizePath(const std::string& _path) const;

	/// @brief フォルダの読み込み
	/// @param _path 読み込むフォルダのパス
	void LoadFolder(const std::string& _path, std::shared_ptr<Folder> _folder);


	/// @brief フォルダーの描画
	/// @param _folder 描画するフォルダ
	void DrawFolderHierarchy(std::shared_ptr<Folder> _folder, size_t _depth);
	void DrawGameFolderHierarchy(std::shared_ptr<Folder> _folder, size_t _depth);

	/// @brief フォルダーの中身を描画
	/// @param _folder 表示するフォルダ
	void DrawFolder(std::shared_ptr<Folder> _folder);

	
	/// 右クリックメニューの表示
	void ShowContextMenu(const std::string& _contextMenuName, const std::string& _currentFolderName);


private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	class AssetCollection* pAssetCollection_; ///< グラフィックスリソースコレクションへのポインタ
	class EditorManager* pEditorManager_; ///< エディターマネージャーへのポインタ

	std::shared_ptr<Folder> assetsRootFolder_;
	std::shared_ptr<Folder> packagesRootFolder_;
	std::shared_ptr<Folder> scriptRootFolder_;
	std::shared_ptr<Folder> selectedFolder_;
	File* selectedFile_ = nullptr; ///< 選択されているファイル
	uint32_t selectedItemPtr_ = 0;
	bool isGameFolder_ = false; // ゲームファイルを選択しているかどうか

	std::string winName_;

};





class ImGuiProjectExplorer : public IImGuiChildWindow {
public:

	struct Entry {
		std::filesystem::path path;
		bool isDirectory;
		std::vector<Entry> children;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiProjectExplorer(class AssetCollection* _assetCollection, class EditorManager* _editorManager);
	~ImGuiProjectExplorer() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	void DrawDirectoryTree(const std::filesystem::path& dir);
	void DrawFileView(const std::filesystem::path& dir);

	/// @brief 右クリックしたときに表示するポップアップメニュー
	/// @param _dir 右クリックしたディレクトリのパス
	void PopupContextMenu(const std::filesystem::path& _dir);

	///// @brief ファイルの名前変更処理
	//void RenameFileOrDirectory();

	void SetRenameMode(const std::filesystem::path& _path);


private:
	class AssetCollection* pAssetCollection_;

	std::filesystem::path rootPath_;
	std::filesystem::path currentPath_;   // 今見ているフォルダ
	std::unordered_map<std::string, bool> dirOpenState_; // ツリーの開閉状態
	std::filesystem::file_time_type lastWriteTime_;

	/// ----- 名前の変更に使う ----- ///
	bool justStartedRename_;
	bool isRenaming_ = false;
	std::filesystem::path renamingPath_;
	std::string renameBuffer_;


};