#pragma once

/// engine
#include "../EditorViewCollection.h"	

/// ///////////////////////////////////////////////////
/// エディターウィンドウ
/// ///////////////////////////////////////////////////
namespace Editor {

class PrefabTab : public IEditorWindowContainer {
public:
	/// ===================================================
	/// public : methods   
	/// ===================================================

	PrefabTab(
		ONEngine::DxManager* _dxm,
		ONEngine::EntityComponentSystem* _ecs, 
		ONEngine::AssetCollection* _assetCollection, 
		EditorManager* _editorManager, 
		ONEngine::SceneManager* _sceneManager
	);
	~PrefabTab() {}

};

} /// Editor
