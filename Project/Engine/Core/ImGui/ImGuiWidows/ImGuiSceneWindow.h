#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiSceneWindow
/// ///////////////////////////////////////////////////
class ImGuiSceneWindow : public IImGuiWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	ImGuiSceneWindow() {}
	~ImGuiSceneWindow() {}
	
	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================


};

