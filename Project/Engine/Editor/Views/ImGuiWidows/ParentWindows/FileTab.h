#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// File操作用のImGuiWindow
/// ///////////////////////////////////////////////////
namespace Editor {

class FileTab : public IEditorWindowContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	FileTab();
	~FileTab() {}
	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

} /// Editor
