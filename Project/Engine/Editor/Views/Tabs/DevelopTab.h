#pragma once

/// editor
#include "../EditorViewCollection.h"

namespace ONEngine {
/// 前方宣言
class DxManager;
class EntityComponentSystem;
class SceneManager;
} // namespace ONEngine

namespace ONEngine::Asset {
class AssetCollection;
}


namespace Editor {

/// 前方宣言
class EditorManager;

/// ///////////////////////////////////////////////////
/// GameWindow
/// ///////////////////////////////////////////////////
class DevelopTab : public IEditorWindowContainer {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DevelopTab(ONEngine::DxManager* _dxm,
		ONEngine::EntityComponentSystem* _ecs,
		ONEngine::Asset::AssetCollection* _assetCollection,
		EditorManager* _editorManager,
		ONEngine::SceneManager* _sceneManager);
	~DevelopTab() {}

};

} // namespace Editor
