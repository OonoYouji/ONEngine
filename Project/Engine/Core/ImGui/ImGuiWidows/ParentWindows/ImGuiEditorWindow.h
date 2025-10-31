#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"	

/// ///////////////////////////////////////////////////
/// エディターウィンドウ
/// ///////////////////////////////////////////////////
class ImGuiEditorWindow : public IImGuiParentWindow {
public:
	/// ===================================================
	/// public : methods   
	/// ===================================================

	ImGuiEditorWindow(class EntityComponentSystem* _ecs, class AssetCollection* _assetCollection, class EditorManager* _editorManager, class SceneManager* _sceneManager);
	~ImGuiEditorWindow() {}

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

