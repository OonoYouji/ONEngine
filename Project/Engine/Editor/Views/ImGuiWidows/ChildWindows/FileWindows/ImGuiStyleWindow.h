#pragma once

#include "../../EditorViewCollection.h"

/// //////////////////////////////////////////////////////
/// ImGuiのスタイル操作Window
/// //////////////////////////////////////////////////////
namespace Editor {

class ImGuiStyleWindow : public IEditorWindow {
public:
	/// ====================================================
	/// public : methods
	/// ====================================================

	ImGuiStyleWindow();
	~ImGuiStyleWindow() {}

	void ShowImGui() override;

};

} /// Editor
