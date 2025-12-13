#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

namespace ONEngine {
class AssetCollection;
}

namespace Editor {

/// ///////////////////////////////////////////////////
/// GameSceneTextureを表示するためのImGuiWindow
/// ///////////////////////////////////////////////////
class ImGuiGameSceneWindow : public IEditorView {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	ImGuiGameSceneWindow(ONEngine::AssetCollection* _assetCollection) : pAssetCollection_(_assetCollection) {}
	~ImGuiGameSceneWindow() {}
	
	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	ONEngine::AssetCollection* pAssetCollection_ = nullptr;

};

} /// Editor
