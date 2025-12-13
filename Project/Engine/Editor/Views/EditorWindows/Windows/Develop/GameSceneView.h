#pragma once

/// engine
#include "../../EditorViewCollection.h"

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
	GameSceneView(ONEngine::AssetCollection* _ac, const std::string& _windowName)
		: pAssetCollection_(_ac), windowName_(_windowName) {
	}
	~GameSceneView() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	ONEngine::AssetCollection* pAssetCollection_ = nullptr;
	const std::string windowName_ = "GameView";

};

} /// Editor
