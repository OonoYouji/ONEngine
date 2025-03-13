#pragma once

/// engine
#include "Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiMainWindow
/// ///////////////////////////////////////////////////
class ImGuiMainWindow : public IImGuiWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiMainWindow() {}
	~ImGuiMainWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void FileMenu();

	void EffectMenu();

};

