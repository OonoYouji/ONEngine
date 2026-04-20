#pragma once

/// engine
#include "../../EditorViewCollection.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Asset/Guid/Guid.h"

namespace ONEngine {
class EntityComponentSystem;
}

namespace ONEngine::Asset {
class AssetCollection;
class Texture;
}


namespace Editor {

class InspectorWindow;

/// //////////////////////////////////////////////////////
/// PrefabFileを表示するためのImGuiWindow
/// //////////////////////////////////////////////////////
class PrefabFileWindow : public IEditorWindow {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	PrefabFileWindow(ONEngine::EntityComponentSystem* _ecs, ONEngine::Asset::AssetCollection* _assetCollection, InspectorWindow* _inspector);
	~PrefabFileWindow() override = default;

	void ShowImGui() override;


	/// @brief ファイルリストの表示
	void ShowPrefabFileList();

	/// @brief Prefabファイルを再読み込みする
	/// @param _tex ImageButtonに使うテクスチャ
	void ReloadPrefabFiles(const ONEngine::Asset::Texture* _tex);

	/// @brief 新規Prefab作成ウィンドウ
	void AddNewPrefabWindow();

	/// @brief 新規のPrefabを作成す
	/// @return true: 作成成功, false: 作成失敗
	bool GenerateNewPrefab();

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	/// --------------- other class pointers --------------- ///
	ONEngine::EntityComponentSystem* pEcs_;
	ONEngine::Asset::AssetCollection* pAssetCollection_;
	InspectorWindow* pInspector_;


	/// --------------- member objects --------------- ///
	std::string       searchText_; ///< 検索テキスト
	std::vector<ONEngine::File> files_;

	/// 現在選択されているPrefabのEntity
	ONEngine::Guid selectedPrefabGuid_;


	/// --------------- add prefab --------------- ///

	std::string newPrefabName_; ///< 新規Prefabの名前

};

} /// Editor
