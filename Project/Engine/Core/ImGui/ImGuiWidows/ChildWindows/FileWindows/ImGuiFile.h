#pragma once

#include "../../Collection/ImGuiWindowCollection.h"

/// //////////////////////////////////////////////////////
/// ImGuiのスタイル操作Window
/// //////////////////////////////////////////////////////
class ImGuiFile : public IImGuiChildWindow {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	ImGuiFile();
	~ImGuiFile() {}

	void ShowImGui() override;

private:
	/// ====================================================
	/// private : objects
	/// ====================================================

};

