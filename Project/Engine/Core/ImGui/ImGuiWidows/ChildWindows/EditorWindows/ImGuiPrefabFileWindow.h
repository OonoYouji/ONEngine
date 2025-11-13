#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"
#include "Engine/Core/Utility/Math/Mathf.h"
#include "Engine/Asset/Guid/Guid.h"

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


	/// @brief ファイルリストの表示
	void ShowPrefabFileList();

	/// @brief Prefabファイルを再読み込みする
	/// @param _tex ImageButtonに使うテクスチャ
	void ReloadPrefabFiles(const class Texture* _tex);

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
	class EntityComponentSystem* pEcs_;
	class ImGuiInspectorWindow*  pInspector_;
	class AssetCollection*       pAssetCollection_;


	/// --------------- member objects --------------- ///
	std::string       searchText_; ///< 検索テキスト
	std::vector<File> files_;

	/// 現在選択されているPrefabのEntity
	Guid selectedPrefabGuid_;


	/// --------------- add prefab --------------- ///

	std::string newPrefabName_; ///< 新規Prefabの名前

};

