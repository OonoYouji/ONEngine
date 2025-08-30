#pragma once

#include "../../Collection/ImGuiWindowCollection.h"


/// //////////////////////////////////////////////////////
/// Prefab編集用のimgui window
/// //////////////////////////////////////////////////////
class ImGuiPrefabViewWindow : public IImGuiChildWindow {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	ImGuiPrefabViewWindow(class EntityComponentSystem* _ecs, class GraphicsResourceCollection* _resourceCollection);
	~ImGuiPrefabViewWindow() {}

	void ShowImGui() override;

	void RenderView();

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	class EntityComponentSystem* pECS_;
	class GraphicsResourceCollection* pResourceCollection_;


};

