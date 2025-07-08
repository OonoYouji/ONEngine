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

	ImGuiPrefabFileWindow(class EntityComponentSystem* _ecs, class ImGuiPrefabInspectorWindow* _inspector);
	~ImGuiPrefabFileWindow() override = default;

	void ImGuiFunc() override;

private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	class EntityComponentSystem* pECS_ = nullptr; ///< EntityComponentSystemへのポインタ
	class ImGuiPrefabInspectorWindow* pInspector_ = nullptr; ///< Prefab Inspectorへのポインタ

	std::string searchText_; ///< 検索テキスト
	std::vector<File> files_;

};

