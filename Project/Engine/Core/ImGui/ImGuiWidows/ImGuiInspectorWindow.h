#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiInspectorWindow
/// ///////////////////////////////////////////////////
class ImGuiInspectorWindow : public IImGuiWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiInspectorWindow() {}
	~ImGuiInspectorWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

};

