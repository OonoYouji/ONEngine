#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiSceneWindow
/// ///////////////////////////////////////////////////
class ImGuiSceneWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	ImGuiSceneWindow(class EntityComponentSystem* _ecs, class GraphicsResourceCollection* _graphicsResourceCollection, class SceneManager* _sceneManager, class ImGuiInspectorWindow* _inspector);
	~ImGuiSceneWindow() {}
	
	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	class EntityComponentSystem* pECS_;
	class GraphicsResourceCollection* resourceCollection_ = nullptr;
	class SceneManager* pSceneManager_ = nullptr;
	class ImGuiInspectorWindow* pInspector_ = nullptr;

	int manipulateOperation_;
	int manipulateMode_;
};

