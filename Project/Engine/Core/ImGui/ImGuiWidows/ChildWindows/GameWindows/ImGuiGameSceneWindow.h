#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiGameWindow
/// ///////////////////////////////////////////////////
class ImGuiGameSceneWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ImGuiGameSceneWindow(class GraphicsResourceCollection* _graphicsResourceCollection) : resourceCollection_(_graphicsResourceCollection) {}
	~ImGuiGameSceneWindow() {}
	
	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	class GraphicsResourceCollection* resourceCollection_ = nullptr;

};

