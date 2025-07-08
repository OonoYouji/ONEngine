#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"	

/// ///////////////////////////////////////////////////
/// エディターウィンドウ
/// ///////////////////////////////////////////////////
class ImGuiEditorWindow : public IImGuiParentWindow {
public:
	/// ===================================================
	/// public : methods   
	/// ===================================================

	ImGuiEditorWindow(class GraphicsResourceCollection* _resourceCollection);
	~ImGuiEditorWindow() {}

	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;

};

