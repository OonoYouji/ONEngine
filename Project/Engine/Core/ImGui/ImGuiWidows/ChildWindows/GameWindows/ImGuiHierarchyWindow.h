#pragma once

/// std
#include <string>
#include <list>

/// engine
#include "../../Collection/ImGuiWindowCollection.h"

/// ///////////////////////////////////////////////////
/// ImGuiHierarchyWindow
/// ///////////////////////////////////////////////////
class ImGuiHierarchyWindow : public IImGuiChildWindow {
public:
	/// ===================================================
	/// public : methods   
	/// ===================================================

	ImGuiHierarchyWindow(class EntityComponentSystem*, class EditorManager*, class SceneManager*, class ImGuiInspectorWindow*);
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

	void EntityDebug(class IEntity* _entity);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string entityName_ = "empty";
	const std::string kClassPrefix = "class ";

	std::list<class IEntity*> entityList_;


	class EntityComponentSystem* pEntityComponentSystem_ = nullptr;
	class EditorManager* pEditorManager_ = nullptr;
	class SceneManager* pSceneManager_ = nullptr;
	class ImGuiInspectorWindow* pInspectorWindow_ = nullptr;
	class IEntity* selectedEntity_ = nullptr;

	/* ----- hierarchy ----- */
	bool isNodeOpen_;
	std::string selectedEntityName_ = "empty"; ///< 選択しているエンティティの名前

	/* ----- rename ----- */
	std::string newName_ = "";
	IEntity* renameEntity_;

};