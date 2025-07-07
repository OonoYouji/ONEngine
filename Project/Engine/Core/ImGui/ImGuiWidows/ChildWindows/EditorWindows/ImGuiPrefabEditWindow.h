#pragma once

#include "../../Collection/ImGuiWindowCollection.h"


/// //////////////////////////////////////////////////////
/// Prefab編集用のimgui window
/// //////////////////////////////////////////////////////
class ImGuiPrefabEditWindow : public IImGuiChildWindow {
public:
	/// =====================================================
	/// public : methods
	/// =====================================================

	ImGuiPrefabEditWindow();
	~ImGuiPrefabEditWindow() {}

	void ImGuiFunc() override;


private:
	/// =====================================================
	/// private : objects
	/// =====================================================




};

