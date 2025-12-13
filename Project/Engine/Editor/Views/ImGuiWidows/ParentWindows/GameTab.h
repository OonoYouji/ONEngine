#pragma once

/// editor
#include "../ImGuiWindowCollection.h"

namespace ONEngine {
/// 前方宣言
class DxManager;
class EntityComponentSystem;
class AssetCollection;
class SceneManager;
} // namespace ONEngine

namespace Editor {

/// 前方宣言
class EditorManager;

/// ///////////////////////////////////////////////////
/// GameWindow
/// ///////////////////////////////////////////////////
class GameTab : public IEditorWindowContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	GameTab(ONEngine::DxManager* _dxm,
		ONEngine::EntityComponentSystem* _ecs,
		ONEngine::AssetCollection* _assetCollection,
		EditorManager* _editorManager,
		ONEngine::SceneManager* _sceneManager);
	~GameTab() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

} // namespace Editor
