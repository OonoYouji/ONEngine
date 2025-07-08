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

	ImGuiPrefabEditWindow(class GraphicsResourceCollection* _resourceCollection);
	~ImGuiPrefabEditWindow() {}

	void ImGuiFunc() override;


private:
	/// =====================================================
	/// private : objects
	/// =====================================================

	class GraphicsResourceCollection* pResourceCollection_ = nullptr;


};

