#pragma once

#include "../../EditorViewCollection.h"

namespace ONEngine {
class EntityComponentSystem;
}

namespace ONEngine::Asset {
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

	PrefabViewWindow(ONEngine::EntityComponentSystem* _ecs, ONEngine::Asset::AssetCollection* _assetCollection);
	~PrefabViewWindow() {}

	void ShowImGui() override;

	/// @brief 選択されているPrefabの表示
	void RenderView();

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	ONEngine::EntityComponentSystem* pEcs_;
	ONEngine::Asset::AssetCollection* pAssetCollection_;


};

} /// Editor
