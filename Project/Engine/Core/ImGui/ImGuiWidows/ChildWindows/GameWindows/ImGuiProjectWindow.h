#pragma once

#include <string>

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

	ImGuiProjectWindow(class GraphicsResourceCollection* _graphicsResourceCollection, class EditorManager* _editorManager);
	~ImGuiProjectWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

	void SetWindowName(const std::string& _name);

	void ReloadProject();

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
	
	/// @brief プロジェクトの階層表示
	void Hierarchy();

	/// @brief プロジェクトのファイル表示
	void SelectFileView();

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

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	class GraphicsResourceCollection* pGraphicsResourceCollection_; ///< グラフィックスリソースコレクションへのポインタ
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



