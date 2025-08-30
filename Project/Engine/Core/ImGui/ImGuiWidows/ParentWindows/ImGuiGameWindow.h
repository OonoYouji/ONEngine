#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiGameWindow
/// ///////////////////////////////////////////////////
class ImGuiGameWindow : public IImGuiParentWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiGameWindow(class EntityComponentSystem*, class GraphicsResourceCollection*, class EditorManager*, class SceneManager*);
	~ImGuiGameWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

