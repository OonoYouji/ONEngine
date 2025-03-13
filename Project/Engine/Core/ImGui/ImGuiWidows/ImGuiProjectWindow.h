#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiProjectWindow
/// ///////////////////////////////////////////////////
class ImGuiProjectWindow : public IImGuiWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiProjectWindow() {}
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

};

