#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiMainWindow
/// ///////////////////////////////////////////////////
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

	void MainMenu();

	void EffectMenu();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int currentMenuIndex_ = 0;

};

