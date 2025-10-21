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

	ImGuiPrefabViewWindow(class EntityComponentSystem* _ecs, class AssetCollection* _resourceCollection);
	~ImGuiPrefabViewWindow() {}

	void ShowImGui() override;

	/// @brief 選択されているPrefabの表示
	void RenderView();

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	class EntityComponentSystem* pEcs_;
	class AssetCollection* pGrc_;


};

