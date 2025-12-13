#pragma once

/// engine
#include "../../ImGuiWindowCollection.h"

namespace ONEngine {
class AssetCollection;
}

namespace Editor {

/// ///////////////////////////////////////////////////
/// GameSceneTextureを表示するためのImGuiWindow
/// ///////////////////////////////////////////////////
class GameSceneView : public IEditorWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	GameSceneView(ONEngine::AssetCollection* _assetCollection) : pAssetCollection_(_assetCollection) {}
	~GameSceneView() {}
	
	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	ONEngine::AssetCollection* pAssetCollection_ = nullptr;

};

} /// Editor
