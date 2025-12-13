#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// File操作用のImGuiWindow
/// ///////////////////////////////////////////////////
namespace Editor {

class ImGuiFileWindow : public IEditorViewContainer {
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

} /// Editor
