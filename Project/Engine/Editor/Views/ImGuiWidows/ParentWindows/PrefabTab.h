#pragma once

/// engine
#include "../ImGuiWindowCollection.h"	

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

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

} /// Editor
