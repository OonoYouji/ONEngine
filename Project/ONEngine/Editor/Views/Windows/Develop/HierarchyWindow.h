#pragma once

/// std
#include <string>

/// engine
#include "../../EditorViewCollection.h"
#include "Engine/Asset/Guid/Guid.h"

namespace ONEngine {
/// 前方宣言
class ECSGroup;
class GameEntity;
class EntityComponentSystem;
class SceneManager;
}

namespace Editor {

/// @brief 前方宣言
class EditorManager;

/// ///////////////////////////////////////////////////
/// ECSGroupのヒエラルキーを表示
/// ///////////////////////////////////////////////////
class HierarchyWindow : public IEditorWindow {
public:
	/// ===================================================
	/// public : methods   
	/// ===================================================

	HierarchyWindow(const std::string& windowName, ONEngine::EntityComponentSystem* ecs, ONEngine::ECSGroup* ecsGroup, EditorManager* editorManager, ONEngine::SceneManager* sceneManager);
	~HierarchyWindow() override = default;

	/// @brief GUIの表示
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
	void EntityRename(ONEngine::GameEntity* _entity);

	/// Dialogの表示
	void DrawDialog();
	void DrawSceneSaveDialog();

	/// ----- element methods ----- ///

	/// @brief 親子関係のループチェック
	bool IsDescendant(ONEngine::GameEntity* ancestor, ONEngine::GameEntity* descendant);

	/// @brief エラーポップアップの表示
	void ShowInvalidParentPopup();


	///	--------------------------------------------------------------------------------------------------
	/// エンティティのエディタ表示
	///	--------------------------------------------------------------------------------------------------

	/// @brief エンティティのエディタ表示
	/// @param entity 表示対象のエンティティ
	void DrawEntity(ONEngine::GameEntity* entity);

	void HandleRootDragDrop();

	void HandleEntityDragDrop(ONEngine::GameEntity* entity);

	void DrawEntityContextMenu(ONEngine::GameEntity* entity, bool selected);

	void HandleEntityShortcuts(ONEngine::GameEntity* entity, bool selected);


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	/// ----- other class ----- ///
	ONEngine::EntityComponentSystem* pEcs_ = nullptr;
	ONEngine::ECSGroup* pEcsGroup_ = nullptr;
	EditorManager* pEditorManager_ = nullptr;
	ONEngine::SceneManager* pSceneManager_ = nullptr;

	std::string windowName_ = "Hierarchy";

	/// ----- hierarchy ----- ///
	bool isNodeOpen_;

	/// ----- rename ----- ///
	std::string newName_ = "";
	ONEngine::Guid renameEntityGuid_; ///< 生ポインタではなくGuidで安全に管理

	/// ----- test objects ----- ///
	bool showInvalidParentPopup_ = false;

};

/// ///////////////////////////////////////////////////
/// 通常のシーンのHierarchyウィンドウ
/// ///////////////////////////////////////////////////
class NormalHierarchyWindow : public HierarchyWindow {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	NormalHierarchyWindow(const std::string& windowName, ONEngine::EntityComponentSystem* ecs, EditorManager* editorManager, ONEngine::SceneManager* sceneManager);
	~NormalHierarchyWindow() override = default;

	void ShowImGui() override;

	/// ----- dialog ----- ///
	void DrawSceneDialog();

	void HandleGlobalShortcuts();

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	ONEngine::EntityComponentSystem* pEcs_ = nullptr;
};

} /// Editor