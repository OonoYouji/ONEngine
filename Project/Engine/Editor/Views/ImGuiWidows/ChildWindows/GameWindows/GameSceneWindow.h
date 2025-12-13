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
class GameSceneWindow : public IEditorWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	GameSceneWindow(ONEngine::AssetCollection* _assetCollection) : pAssetCollection_(_assetCollection) {}
	~GameSceneWindow() {}
	
	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	ONEngine::AssetCollection* pAssetCollection_ = nullptr;

};

} /// Editor
