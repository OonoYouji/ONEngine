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

	ImGuiHierarchyWindow(class EntityCollection* _entityCollection);
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

	class EntityCollection* entityCollection_ = nullptr;
	class IEntity* selectedEntity_ = nullptr;
};