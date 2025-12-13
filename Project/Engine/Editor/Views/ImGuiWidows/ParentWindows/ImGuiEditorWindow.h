#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"	

/// ///////////////////////////////////////////////////
/// エディターウィンドウ
/// ///////////////////////////////////////////////////
namespace Editor {

class ImGuiEditorWindow : public IEditorViewContainer {
public:
	/// ===================================================
	/// public : methods   
	/// ===================================================

	ImGuiEditorWindow(
		ONEngine::DxManager* _dxm,
		ONEngine::EntityComponentSystem* _ecs, 
		ONEngine::AssetCollection* _assetCollection, 
		EditorManager* _editorManager, 
		ONEngine::SceneManager* _sceneManager
	);
	~ImGuiEditorWindow() {}

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

} /// Editor
