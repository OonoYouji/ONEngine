#pragma once

#include "../../Collection/ImGuiWindowCollection.h"

/// //////////////////////////////////////////////////////
/// ImGuiのスタイル操作Window
/// //////////////////////////////////////////////////////
namespace Editor {

class ImGuiFile : public IImGuiChildWindow {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	ImGuiFile();
	~ImGuiFile() {}

	void ShowImGui() override;

};

} /// Editor
