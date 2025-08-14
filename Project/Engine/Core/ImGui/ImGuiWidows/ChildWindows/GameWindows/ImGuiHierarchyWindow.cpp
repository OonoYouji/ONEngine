#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>
#include <dialog/ImGuiFileDialog.h>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Core/Utility/Math/Mathf.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Scene/SceneManager.h"
#include "ImGuiInspectorWindow.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"


ImGuiHierarchyWindow::ImGuiHierarchyWindow(
	const std::string& _imGuiWindowName,
	ECSGroup* _ecsGroup,
	EditorManager* _editorManager,
	SceneManager* _sceneManager,
	ImGuiInspectorWindow* _inspectorWindow)
	: imGuiWindowName_(_imGuiWindowName), pECSGroup_(_ecsGroup), pEditorManager_(_editorManager),
	pSceneManager_(_sceneManager), pInspectorWindow_(_inspectorWindow) {

	newName_.reserve(1024);
	isNodeOpen_ = false;
}

void ImGuiHierarchyWindow::ImGuiFunc() {
	if (!ImGui::Begin(imGuiWindowName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	/*/// ドラッグ先の領域を設定
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos());
	ImGui::InvisibleButton("DropTargetArea", ImGui::GetWindowSize());*/

	PrefabDragAndDrop();

	DrawMenuBar();

	/// ヒエラルキーの表示
	DrawHierarchy();

	ImGui::End();
}

void ImGuiHierarchyWindow::PrefabDragAndDrop() {

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			if (payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				if (path.find(".prefab") != std::string::npos) {

					/// pathの文字列をentity名に変換する処理
					std::string str = path;
					size_t pos = str.find_last_of('/');
					if (pos != std::string::npos) {
						str.erase(0, pos + 1);
					}

					pECSGroup_->GenerateEntityFromPrefab(str, DebugConfig::isDebugging);
					Console::Log(std::format("entity name set to: {}", str));
				} else {
					Console::Log("[error] Invalid entity format. Please use \".prefab\"");
				}
			}
		}

		ImGui::EndDragDropTarget();
	}
}

void ImGuiHierarchyWindow::DrawEntityHierarchy(GameEntity* _entity) {
	entityName_ = _entity->GetName();
	entityName_ += "##" + std::to_string(reinterpret_cast<uintptr_t>(_entity));


	/// 子オブジェクトの表示
	if (_entity->GetChildren().empty()) {
		/// ------------------------------------
		/// 子オブジェクトがいない場合の表示
		/// ------------------------------------

		if (renameEntity_ && renameEntity_ == _entity) {
			EntityRename(_entity);

		} else {

			if (ImGui::Selectable(entityName_.c_str(), _entity == selectedEntity_)) {
				selectedEntity_ = _entity;
			}
		}

		EntityDebug(_entity);

	} else {
		/// ------------------------------------
		/// 子オブジェクトがいる場合の表示
		/// ------------------------------------

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
		selectedEntityName_ = "##";

		/// 名前変更中のエンティティの場合は、名前を表示しない
		if (renameEntity_ && renameEntity_ == _entity) {
			selectedEntityName_ += entityName_;
		} else {
			nodeFlags |= ImGuiTreeNodeFlags_SpanFullWidth;
			selectedEntityName_ = entityName_;
		}


		// 子がいる場合はTreeNodeを使用して階層構造を開閉可能にする  
		isNodeOpen_ = ImGui::TreeNodeEx(selectedEntityName_.c_str(), nodeFlags);
		if (ImGui::IsItemClicked()) {
			selectedEntity_ = _entity;
		}

		if (renameEntity_ && renameEntity_ == _entity) {
			ImGui::SameLine();
			EntityRename(_entity);
		}

		EntityDebug(_entity);


		/// ノードが開いている場合は、子エンティティを再帰的に描画する
		if (isNodeOpen_) {
			ImGui::Indent();
			for (auto& child : _entity->GetChildren()) {
				DrawEntityHierarchy(child);
			}
			ImGui::Unindent();
			ImGui::TreePop();
		}
	}
}

void ImGuiHierarchyWindow::DrawMenuBar() {

	/// 早期リターン
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("+")) {

			/// メニューの表示
			if (ImGui::BeginMenu("create")) {
				if (ImGui::MenuItem("create empty object")) {
					pEditorManager_->ExecuteCommand<CreateGameObjectCommand>(pECSGroup_);
				}

				ImGui::EndMenu();
			}


			if (ImGui::MenuItem("save scene")) {
				pSceneManager_->SaveScene(pECSGroup_->GetGroupName(), pECSGroup_);
			}

			if (ImGui::BeginMenu("load scene")) {
				// open Dialog Simple
				if (ImGui::MenuItem("open explorer")) {
					IGFD::FileDialogConfig config;
					config.path = "./Assets/Scene";
					ImGuiFileDialog::Instance()->OpenDialog("Dialog", "Choose File", ".json", config);
				}

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("scripts")) {

				if (ImGui::MenuItem("hot reload")) {
					pEditorManager_->ExecuteCommand<ReloadAllScriptsCommand>(pECSGroup_, pSceneManager_);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	// display
	if (ImGuiFileDialog::Instance()->Display("Dialog", ImGuiWindowFlags_NoDocking)) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			size_t pos = filePathName.find_last_of("\\");
			if (pos != std::string::npos) {
				filePathName = filePathName.substr(pos + 1); // パスを除去
			}

			pos = filePathName.find_last_of(".");
			if (pos != std::string::npos) {
				filePathName = filePathName.substr(0, pos); // 拡張子を除去
			}

			// action
			pSceneManager_->GetSceneIO()->Input(filePathName, pECSGroup_);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

}

void ImGuiHierarchyWindow::DrawHierarchy() {

	if (ImGui::CollapsingHeader(pECSGroup_->GetGroupName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		/// entityの選択  
		entityList_.clear();
		for (auto& entity : pECSGroup_->GetEntities()) {
			if (!entity->GetParent()) { //!< 親がいない場合  
				entityList_.push_back(entity.get());
			}
		}

		/// entityの表示  
		for (auto& entity : entityList_) {
			DrawEntityHierarchy(entity);
		}

		bool hasValidSelection = false;
		for (auto& entity : pECSGroup_->GetEntities()) {
			if (entity.get() == selectedEntity_) {
				hasValidSelection = true;
				break;
			}
		}

		if (hasValidSelection) {
			pInspectorWindow_->SetSelectedEntity(reinterpret_cast<std::uintptr_t>(selectedEntity_));
		}
	}

}


void ImGuiHierarchyWindow::EntityRename(GameEntity* _entity) {

	if (ImGuiInputText("##rename", &newName_, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_EnterReturnsTrue)) {
		pEditorManager_->ExecuteCommand<EntityRenameCommand>(_entity, newName_);
		renameEntity_ = nullptr;
	}

	// フォーカスが外れたらリネームキャンセル
	if (Input::TriggerMouse(Mouse::Right) || Input::TriggerKey(DIK_ESCAPE)) {
		renameEntity_ = nullptr;
	}
}

void ImGuiHierarchyWindow::EntityDebug(GameEntity* _entity) {
	/// -------------------------------------
	// ドラッグの開始
	/// -------------------------------------
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		ImGui::SetDragDropPayload("ENTITY_HIERARCHY", _entity, sizeof(GameEntity));
		ImGui::TextUnformatted(entityName_.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_HIERARCHY")) {
			GameEntity* srcEntity = static_cast<GameEntity*>(payload->Data);
			if (srcEntity != _entity) {
				//srcEntity->RemoveParent();
				//srcEntity->SetParent(_entity);
			}
		}
		ImGui::EndDragDropTarget();
	}



	/// -------------------------------------
	/// 右クリックしたときのメニューの表示
	/// -------------------------------------
	if (ImGui::BeginPopupContextItem(entityName_.c_str())) {
		if (ImGui::MenuItem("rename")) {
			newName_ = _entity->GetName();
			renameEntity_ = _entity;
		}

		if (ImGui::MenuItem("delete")) {
			pEditorManager_->ExecuteCommand<DeleteEntityCommand>(pECSGroup_, _entity);
			renameEntity_ = nullptr; // 名前変更モードを解除

			/// 選択中ならInspectorの選択を解除
			if (selectedEntity_ == _entity) {
				selectedEntity_ = nullptr;
				pInspectorWindow_->SetSelectedEntity(0); // 選択を解除
			}
		}

		ImGui::EndPopup();
	}



}


/// /////////////////////////////////////////////////////////////////////////
/// ImGuiNormalHierarchyWindow
/// /////////////////////////////////////////////////////////////////////////

ImGuiNormalHierarchyWindow::ImGuiNormalHierarchyWindow(const std::string& _imGuiWindowName, EntityComponentSystem* _ecs, EditorManager* _editorManager, SceneManager* _sceneManager, ImGuiInspectorWindow* _imguiInspectorWindow)
	: ImGuiHierarchyWindow(_imGuiWindowName, nullptr, _editorManager, _sceneManager, _imguiInspectorWindow) {
	pECS_ = _ecs;
}

void ImGuiNormalHierarchyWindow::ImGuiFunc() {
	if (!ImGui::Begin(imGuiWindowName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	pECSGroup_ = pECS_->GetCurrentGroup();

	/*/// ドラッグ先の領域を設定
	ImGui::SetCursorScreenPos(ImGui::GetWindowPos());
	ImGui::InvisibleButton("DropTargetArea", ImGui::GetWindowSize());*/

	PrefabDragAndDrop();

	DrawMenuBar();

	/// ヒエラルキーの表示
	DrawHierarchy();

	ImGui::End();

}
