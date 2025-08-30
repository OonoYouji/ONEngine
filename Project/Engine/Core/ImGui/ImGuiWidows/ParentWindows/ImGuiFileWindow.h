#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"

class ImGuiFileWindow : public IImGuiParentWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ImGuiFileWindow();
	~ImGuiFileWindow() {}
	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;

};

