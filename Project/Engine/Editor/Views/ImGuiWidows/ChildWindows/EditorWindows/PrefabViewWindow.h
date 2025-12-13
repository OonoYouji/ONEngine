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
class PrefabViewWindow : public IEditorWindow {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	PrefabViewWindow(ONEngine::EntityComponentSystem* _ecs, ONEngine::AssetCollection* _assetCollection);
	~PrefabViewWindow() {}

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
