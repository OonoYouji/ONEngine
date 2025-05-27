#pragma once

#include <string>

/// engine
#include "../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiProjectWindow
/// ///////////////////////////////////////////////////
class ImGuiProjectWindow : public IImGuiChildWindow {
	/// ===================================================
	/// private : sub class
	/// ===================================================

	struct File {
		std::string name;
	};

	struct Folder {
		std::string name;
		std::vector<std::shared_ptr<Folder>> folders;
		std::vector<File> files;
	};


public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiProjectWindow();
	~ImGuiProjectWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
	
	/// @brief プロジェクトの階層表示
	void Hierarchy();

	/// @brief プロジェクトのファイル表示
	void SelectFileView();



	/// @brief フォルダの読み込み
	/// @param _path 読み込むフォルダのパス
	void LoadFolder(const std::string& _path, std::shared_ptr<Folder> _folder);


	/// @brief フォルダーの描画
	/// @param _folder 描画するフォルダ
	void DrawFolderHierarchy(std::shared_ptr<Folder> _folder, size_t _depth);

	/// @brief フォルダーの中身を描画
	/// @param _folder 表示するフォルダ
	void DrawFolder(std::shared_ptr<Folder> _folder);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	std::shared_ptr<Folder> assetsRootFolder_;
	std::shared_ptr<Folder> packagesRootFolder_;
	std::shared_ptr<Folder> gameRootFolder_;
	std::shared_ptr<Folder> selectedFolder_;
	uint32_t selectedItemPtr_ = 0;

};



