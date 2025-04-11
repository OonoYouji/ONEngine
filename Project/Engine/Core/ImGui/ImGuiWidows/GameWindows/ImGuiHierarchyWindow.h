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

	ImGuiHierarchyWindow(class EntityComponentSystem* _pEntityComponentSystem);
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
	class IEntity* selectedEntity_ = nullptr;
};