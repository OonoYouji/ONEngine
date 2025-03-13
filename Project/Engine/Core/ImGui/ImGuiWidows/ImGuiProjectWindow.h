#pragma once

#include <string>

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiProjectWindow
/// ///////////////////////////////////////////////////
class ImGuiProjectWindow : public IImGuiWindow {
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



	/// @brief プロジェクトの読み込み
	/// @param _path 読み込むプロジェクトのパス
	void LoadProject(const std::string& _path);

	/// @brief フォルダの読み込み
	/// @param _path 読み込むフォルダのパス
	void LoadFolder(const std::string& _path, std::shared_ptr<Folder> _folder);


	/// @brief フォルダーの描画
	/// @param _folder 描画するフォルダ
	void DrawFolder(std::shared_ptr<Folder> _folder, size_t _depth);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	
	std::shared_ptr<Folder> rootFolder_;

};



