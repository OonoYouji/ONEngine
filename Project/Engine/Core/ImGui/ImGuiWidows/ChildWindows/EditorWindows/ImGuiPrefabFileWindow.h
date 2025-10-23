#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"
#include "Engine/Core/Utility/Math/Mathf.h"

/// //////////////////////////////////////////////////////
/// PrefabFileを表示するためのImGuiWindow
/// //////////////////////////////////////////////////////
class ImGuiPrefabFileWindow : public IImGuiChildWindow {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	ImGuiPrefabFileWindow(class EntityComponentSystem* _ecs, class AssetCollection* _assetCollection, class ImGuiInspectorWindow* _inspector);
	~ImGuiPrefabFileWindow() override = default;

	void ShowImGui() override;

	/// @brief Prefabファイルを再読み込みする
	/// @param _tex ImageButtonに使うテクスチャ
	void ReloadPrefabFiles(const class Texture* _tex);

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	class EntityComponentSystem* pEcs_;
	class ImGuiInspectorWindow* pInspector_;
	class AssetCollection* pAssetCollection_;

	std::string searchText_; ///< 検索テキスト
	std::vector<File> files_;

	/// ----- add prefab ----- /// 

	std::string newPrefabName_; ///< 新規Prefabの名前

};

