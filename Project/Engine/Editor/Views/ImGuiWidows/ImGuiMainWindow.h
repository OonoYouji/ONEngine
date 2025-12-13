#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

namespace Editor {

/// ///////////////////////////////////////////////////
/// ImGuiMainWindow
/// ///////////////////////////////////////////////////
class MainWindow : public IEditorWindowContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	MainWindow() {}
	~MainWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	/// @brief MainMenuの描画
	void MainMenu();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int currentMenuIndex_ = 0;

};


} /// Editor
