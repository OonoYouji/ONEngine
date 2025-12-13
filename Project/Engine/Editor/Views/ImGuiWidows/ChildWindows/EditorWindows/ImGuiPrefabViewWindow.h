#pragma once

#include "../../Collection/ImGuiWindowCollection.h"

namespace ONEngine {
class EntityComponentSystem;
class AssetCollection;
}

namespace Editor {

/// //////////////////////////////////////////////////////
/// Prefab編集用のimgui window
/// //////////////////////////////////////////////////////
class ImGuiPrefabViewWindow : public IEditorView {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	ImGuiPrefabViewWindow(ONEngine::EntityComponentSystem* _ecs, ONEngine::AssetCollection* _assetCollection);
	~ImGuiPrefabViewWindow() {}

	void ShowImGui() override;

	/// @brief 選択されているPrefabの表示
	void RenderView();

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	ONEngine::EntityComponentSystem* pEcs_;
	ONEngine::AssetCollection* pAssetCollection_;


};

} /// Editor
