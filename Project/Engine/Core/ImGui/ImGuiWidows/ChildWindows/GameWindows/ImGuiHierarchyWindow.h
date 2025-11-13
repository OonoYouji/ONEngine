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

	ImGuiHierarchyWindow(const std::string& _imGuiWindowName, class ECSGroup*, class EditorManager*, class SceneManager*);
	~ImGuiHierarchyWindow() override = default;

	void ShowImGui() override;

protected:
	/// ===================================================
	/// protected : methods
	/// ===================================================

	/// @brief Prefabのドラッグアンドドロップ処理
	void PrefabDragAndDrop();
	

	/// ----- menu methods----- ///

	/// @brief MenuBarの描画
	void DrawMenuBar();
	/// @brief Entityメニューの描画
	void DrawMenuEntity();
	/// @brief Sceneメニューの描画
	void DrawMenuScene();

	/// @brief Hierarchyの描画
	void DrawHierarchy();

	/// @brief Entityの名前変更処理
	/// @param _entity 変更対象のEntity
	void EntityRename(class GameEntity* _entity);

	/// Dialogの表示
	void DrawDialog();
	void DrawSceneSaveDialog();


	/// ----- test methods ----- ///

	///
	void DrawEntity(class GameEntity* _entity);

	bool IsDescendant(class GameEntity* _ancestor, class GameEntity* _descendant);

	void ShowInvalidParentPopup();

protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	/// ----- other class ----- ///
	class ECSGroup*             pEcsGroup_        = nullptr;
	class EditorManager*        pEditorManager_   = nullptr;
	class SceneManager*         pSceneManager_    = nullptr;


	std::string       imGuiWindowName_ = "Hierarchy";
	std::string       entityName_      = "empty";
	const std::string kClassPrefix     = "class ";

	std::list<class GameEntity*> entityList_;
	class GameEntity* selectedEntity_ = nullptr;

	/// ----- hierarchy ----- ///
	bool isNodeOpen_;
	std::string selectedEntityName_ = "empty"; ///< 選択しているエンティティの名前

	/// ----- rename ----- ///
	std::string newName_ = "";
	GameEntity* renameEntity_;



	/// ----- test objects ----- ///
	bool 	showInvalidParentPopup_ = false;

};



/// ///////////////////////////////////////////////////
/// 通常のシーンのHierarchyウィンドウ
/// ///////////////////////////////////////////////////
class ImGuiNormalHierarchyWindow : public ImGuiHierarchyWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	ImGuiNormalHierarchyWindow(const std::string& _imGuiWindowName, class EntityComponentSystem* _ecs, class EditorManager* _editorManager, class SceneManager* _sceneManager);
	~ImGuiNormalHierarchyWindow() override = default;
	
	void ShowImGui() override;

	/// ----- dialog ----- ///
	void DrawSceneDialog();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	class EntityComponentSystem* pEcs_ = nullptr;
};
