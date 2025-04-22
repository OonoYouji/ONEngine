#pragma once

/// std
#include <string>
#include <list>

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
		
	void DrawChildren(class IEntity* _entity);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	 
	std::string entityName_ = "empty";
	const std::string kClassPrefix = "class ";

	std::list<class IEntity*> entityList_;


	class EntityComponentSystem* pEntityComponentSystem_ = nullptr;
	class ImGuiInspectorWindow* pInspectorWindow_ = nullptr;
	class IEntity* selectedEntity_ = nullptr;
};