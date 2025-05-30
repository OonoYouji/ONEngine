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

	ImGuiHierarchyWindow(class EntityComponentSystem* _pEntityComponentSystem, class EditorManager* _editorManager, class ImGuiInspectorWindow* _inspectorWindow);
	~ImGuiHierarchyWindow() override = default;

	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================
		
	void DrawEntityHierarchy(class IEntity* _entity);

	void MenuBar();

	void Hierarchy();

	void EntityRename(class IEntity* _entity);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================
	 
	std::string entityName_ = "empty";
	const std::string kClassPrefix = "class ";

	std::list<class IEntity*> entityList_;


	class EntityComponentSystem* pEntityComponentSystem_ = nullptr;
	class EditorManager* pEditorManager_ = nullptr;
	class ImGuiInspectorWindow* pInspectorWindow_ = nullptr;
	class IEntity* selectedEntity_ = nullptr;

	/* ----- hierarchy ----- */
	bool isNodeOpen_;

	/* ----- rename ----- */
	std::string newName_ = "";
	IEntity* renameEntity_;

};