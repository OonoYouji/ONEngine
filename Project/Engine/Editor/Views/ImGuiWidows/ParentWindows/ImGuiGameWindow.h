#pragma once

/// editor
#include "../Collection/ImGuiWindowCollection.h"

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
class ImGuiGameWindow : public IImGuiParentWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiGameWindow(ONEngine::DxManager* _dxm,
		ONEngine::EntityComponentSystem* _ecs,
		ONEngine::AssetCollection* _assetCollection,
		EditorManager* _editorManager,
		ONEngine::SceneManager* _sceneManager);
	~ImGuiGameWindow() {}

	/// @brief imgui windowの描画処理
	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

} // namespace Editor
