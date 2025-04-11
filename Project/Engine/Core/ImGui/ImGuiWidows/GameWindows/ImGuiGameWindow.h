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

	ImGuiGameWindow(class EntityComponentSystem* _pEntityComponentSystem, class GraphicsResourceCollection* _resourceCollection);
	~ImGuiGameWindow() {}

	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

