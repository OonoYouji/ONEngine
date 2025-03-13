#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiGameWindow
/// ///////////////////////////////////////////////////
class ImGuiGameWindow : public IImGuiWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ImGuiGameWindow() {}
	~ImGuiGameWindow() {}
	
	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

};

