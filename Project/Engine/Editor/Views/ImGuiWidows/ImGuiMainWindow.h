﻿#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiMainWindow
/// ///////////////////////////////////////////////////
namespace ONEngine {

class ImGuiMainWindow : public IImGuiParentWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiMainWindow() {}
	~ImGuiMainWindow() {}

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


} /// ONEngine
