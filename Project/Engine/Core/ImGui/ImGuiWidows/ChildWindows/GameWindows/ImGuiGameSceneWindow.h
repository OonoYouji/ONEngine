#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// GameSceneTextureを表示するためのImGuiWindow
/// ///////////////////////////////////////////////////
class ImGuiGameSceneWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ImGuiGameSceneWindow(class AssetCollection* _grc) : pAssetCollection_(_grc) {}
	~ImGuiGameSceneWindow() {}
	
	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	class AssetCollection* pAssetCollection_ = nullptr;

};

