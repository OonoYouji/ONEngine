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

	ImGuiHierarchyWindow(const std::string& _imGuiWindowName, class ECSGroup*, class EditorManager*, class SceneManager*, class ImGuiInspectorWindow*);
	~ImGuiHierarchyWindow() override = default;

	void ImGuiFunc() override;

private:
	/// ===================================================
	/// private : methods
	/// ===================================================

	void DrawEntityHierarchy(class GameEntity* _entity);

	void MenuBar();

	void Hierarchy();

	void EntityRename(class GameEntity* _entity);

	void EntityDebug(class GameEntity* _entity);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string imGuiWindowName_ = "Hierarchy";
	std::string entityName_ = "empty";
	const std::string kClassPrefix = "class ";

	std::list<class GameEntity*> entityList_;


	class ECSGroup* pECSGroup_ = nullptr;
	class EditorManager* pEditorManager_ = nullptr;
	class SceneManager* pSceneManager_ = nullptr;
	class ImGuiInspectorWindow* pInspectorWindow_ = nullptr;
	class GameEntity* selectedEntity_ = nullptr;

	/* ----- hierarchy ----- */
	bool isNodeOpen_;
	std::string selectedEntityName_ = "empty"; ///< 選択しているエンティティの名前

	/* ----- rename ----- */
	std::string newName_ = "";
	GameEntity* renameEntity_;

};