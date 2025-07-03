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

	ImGuiEditorWindow();
	~ImGuiEditorWindow() {}

	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	int imGuiFlags_ = 0;

};

