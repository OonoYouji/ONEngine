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

	ImGuiEditorWindow(class EntityComponentSystem* _ecs, class GraphicsResourceCollection* _resourceCollection, class EditorManager* _editorManager);
	~ImGuiEditorWindow() {}

	void ShowImGui() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;
};

