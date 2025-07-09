#pragma once

#include "../../Collection/ImGuiWindowCollection.h"

/// //////////////////////////////////////////////////////
/// imguiのファイル操作window
/// //////////////////////////////////////////////////////
class ImGuiFile : public IImGuiChildWindow {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	ImGuiFile();
	~ImGuiFile() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ====================================================
	/// private : objects
	/// ====================================================

};

