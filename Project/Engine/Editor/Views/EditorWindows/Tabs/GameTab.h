#pragma once

/// editor
#include "../EditorViewCollection.h"

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

};

} // namespace Editor
