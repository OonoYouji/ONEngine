#pragma once

/// engine
#include "../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiHierarchyWindow
/// ///////////////////////////////////////////////////
class ImGuiHierarchyWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods   
	/// ===================================================

	ImGuiHierarchyWindow(class EntityComponentSystem* _pEntityComponentSystem, class ImGuiInspectorWindow* _inspectorWindow);
	~ImGuiHierarchyWindow() override = default;

	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
		

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class EntityComponentSystem* pEntityComponentSystem_ = nullptr;
	class ImGuiInspectorWindow* pInspectorWindow_ = nullptr;
	class IEntity* selectedEntity_ = nullptr;
};