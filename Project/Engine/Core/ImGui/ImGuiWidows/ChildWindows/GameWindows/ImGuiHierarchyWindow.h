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

	void ShowImGui() override;

protected:
	/// ===================================================
	/// protected : methods
	/// ===================================================

	void PrefabDragAndDrop();
	
	void DrawEntityHierarchy(class GameEntity* _entity);

	/// ----- menu methods----- ///
	void DrawMenuBar();
	void DrawMenuEntity();
	void DrawMenuScene();
	void DrawMenuScript();

	void DrawHierarchy();

	void EntityRename(class GameEntity* _entity);

	void EntityDebug(class GameEntity* _entity);

	/// dialog
	void DrawDialog();
	void DrawSceneSaveDialog();

protected:
	/// ===================================================
	/// protected : objects
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



/// ///////////////////////////////////////////////////
/// 通常のシーンのhierarchyウィンドウ
/// ///////////////////////////////////////////////////
class ImGuiNormalHierarchyWindow : public ImGuiHierarchyWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiNormalHierarchyWindow(const std::string& _imGuiWindowName, class EntityComponentSystem* _ecs, class EditorManager* _editorManager, class SceneManager* _sceneManager, class ImGuiInspectorWindow* _imguiInspectorWindow);
	~ImGuiNormalHierarchyWindow() override = default;
	
	void ShowImGui() override;

	/// ----- dialog ----- ///
	void DrawSceneDialog();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class EntityComponentSystem* pECS_ = nullptr;
};
