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
	
	ImGuiSceneWindow(class GraphicsResourceCollection* _graphicsResourceCollection, class EntityComponentSystem* _ecs, class ImGuiInspectorWindow* _inspector);
	~ImGuiSceneWindow() {}
	
	/// @brief imgui windowの描画処理
	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	class GraphicsResourceCollection* resourceCollection_ = nullptr;
	class EntityComponentSystem* pECS_;
	class ImGuiInspectorWindow* pInspector_ = nullptr;

	int manipulateOperation_;
	int manipulateMode_;
};

