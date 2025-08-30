#pragma once

/// engine
#include "../../Collection/ImGuiWindowCollection.h"
#include "Engine/Core/Utility/Math/Mathf.h"

/// //////////////////////////////////////////////////////
/// Prefab編集用のimgui window
/// //////////////////////////////////////////////////////
class ImGuiPrefabFileWindow : public IImGuiChildWindow {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	ImGuiPrefabFileWindow(class EntityComponentSystem* _ecs, class GraphicsResourceCollection* _resourceCollection, class ImGuiPrefabInspectorWindow* _inspector);
	~ImGuiPrefabFileWindow() override = default;

	void ShowImGui() override;

	void ReloadPrefabFiles(const class Texture* _tex);
	void AddPrefabButton();

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	class EntityComponentSystem* pECS_;
	class ImGuiPrefabInspectorWindow* pInspector_;
	class GraphicsResourceCollection* pResourceCollection_;

	std::string searchText_; ///< 検索テキスト
	std::vector<File> files_;

	/// ----- add prefab ----- /// 

	std::string newPrefabName_; ///< 新規Prefabの名前

};

