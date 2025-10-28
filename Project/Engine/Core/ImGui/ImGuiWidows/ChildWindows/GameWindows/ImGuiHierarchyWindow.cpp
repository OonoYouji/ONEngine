#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>
#include <dialog/ImGuiFileDialog.h>
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Math/Mathf.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"
#include "Engine/Editor/EditCommand.h"
#include "Engine/Scene/SceneManager.h"

/// engine/imgui
#include "ImGuiInspectorWindow.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/Core/ImGui/ImGuiSelection.h"


ImGuiHierarchyWindow::ImGuiHierarchyWindow(
	const std::string& _imGuiWindowName,
	ECSGroup* _ecsGroup,
	EditorManager* _editorManager,
	SceneManager* _sceneManager)
	: imGuiWindowName_(_imGuiWindowName), pECSGroup_(_ecsGroup), pEditorManager_(_editorManager),
	pSceneManager_(_sceneManager) {

	newName_.reserve(1024);
	isNodeOpen_ = false;
}

void ImGuiHierarchyWindow::ShowImGui() {
	if (!ImGui::Begin(imGuiWindowName_.c_str(), nullptr)) {
		ImGui::End();
		return;
	}

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

					pECSGroup_->GenerateEntityFromPrefab(str, GenerateGuid(), DebugConfig::isDebugging);
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

			/// コピーの処理、 アイテムを選択している場合のみコピー可能にする
			if (_entity == selectedEntity_) {

				/// 入力の処理
				if (Input::PressKey(DIK_LCONTROL) || Input::PressKey(DIK_RCONTROL)) {
					if (Input::TriggerKey(DIK_C)) {
						EditCommand::Execute<CopyEntityCommand>(_entity);
					}
				}
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

			DrawMenuEntity();
			DrawMenuScene();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	// display
	DrawDialog();
	DrawSceneSaveDialog();
}

void ImGuiHierarchyWindow::DrawMenuEntity() {
	if (ImGui::BeginMenu("create")) {
		if (ImGui::MenuItem("create empty object")) {
			pEditorManager_->ExecuteCommand<CreateGameObjectCommand>(pECSGroup_);
		}

		ImGui::EndMenu();
	}
}

void ImGuiHierarchyWindow::DrawMenuScene() {
	if (ImGui::BeginMenu("scene")) {

		if (ImGui::MenuItem("create scene")) {
			/// 新規のシーンファイルを作成する
			IGFD::FileDialogConfig config;
			config.path = "./Assets/Scene";
			ImGuiFileDialog::Instance()->OpenDialog("save file dialog", "ファイル保存", ".json", config);
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

		ImGui::EndMenu();
	}
}

void ImGuiHierarchyWindow::DrawHierarchy() {

	/// ECSGroupないにあるEntityの表示
	if (ImGui::CollapsingHeader(pECSGroup_->GetGroupName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		/// 透明なボタンを表示し、ここにエンティティがドロップされたら親子関係を解除する
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		windowSize.y = 12.0f;
		ImGui::InvisibleButton("HierarchyDropArea", windowSize);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityData")) {
				GameEntity** srcEntityPtr = static_cast<GameEntity**>(payload->Data);
				GameEntity* srcEntity = *srcEntityPtr;
				/// 親子関係の解除
				srcEntity->RemoveParent();
			}
			ImGui::EndDragDropTarget();
		}


		/// ---------------------------------------------------
		/// エンティティの表示
		/// ---------------------------------------------------

		for (auto& entity : pECSGroup_->GetEntities()) {
			/// 子を再帰的に処理するので親がないエンティティだけ処理する
			if (!entity->GetParent()) {
				DrawEntity(entity.get());
			}
		}

		/// 無効な親子関係のポップアップ表示
		ShowInvalidParentPopup();

		///// entityの選択  
		//entityList_.clear();
		//for (auto& entity : pECSGroup_->GetEntities()) {
		//	if (!entity->GetParent()) { //!< 親がいない場合  
		//		entityList_.push_back(entity.get());
		//	}
		//}

		///// entityの表示  
		//for (auto& entity : entityList_) {
		//	DrawEntityHierarchy(entity);
		//}

		//bool hasValidSelection = false;
		//for (auto& entity : pECSGroup_->GetEntities()) {
		//	if (entity.get() == selectedEntity_) {
		//		hasValidSelection = true;
		//		break;
		//	}
		//}

		//if (hasValidSelection) {
		//	/// ----- 選択したオブジェクトを設定 ----- ///
		//	ImGuiSelection::SetSelectedObject(selectedEntity_->GetGuid(), SelectionType::Entity);
		//}
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
				ImGuiSelection::SetSelectedObject(Guid::kInvalid, SelectionType::None);
			}
		}

		ImGui::EndPopup();
	}



}

void ImGuiHierarchyWindow::DrawDialog() {
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
			pECSGroup_->RemoveEntityAll();
			pSceneManager_->GetSceneIO()->Input(filePathName, pECSGroup_);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

}

void ImGuiHierarchyWindow::DrawSceneSaveDialog() {
	if (ImGuiFileDialog::Instance()->Display("save file dialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			/// 空のJSONオブジェクト
			nlohmann::json j = nlohmann::json::object();

			/// ファイルの作成
			std::ofstream ofs(filePathName, std::ios::out | std::ios::binary);
			if (ofs) {
				ofs << j.dump(4);
				ofs.close();
			} else {
				Console::LogError("Failed to create file: " + filePathName);
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

bool ImGuiHierarchyWindow::DrawEntity(GameEntity* _entity) {
	/// ----- Entityの表示 ----- ///

	/// ---------------------------------------------------
	/// Selectableでエンティティ名の表示
	/// ---------------------------------------------------

	/// 名前被りでエラーになるのを防ぐため、IDを付与
	ImGui::PushID(_entity->GetId());
	bool isSelected = ImGui::Selectable(_entity->GetName().c_str(), _entity == selectedEntity_);
	ImGui::PopID();

	if (isSelected) {
		ImGuiSelection::SetSelectedObject(_entity->GetGuid(), SelectionType::Entity);
	}


	/// ---------------------------------------------------
	/// ドラッグの開始
	/// ---------------------------------------------------

	if (ImGui::BeginDragDropSource()) {
		ImGui::Text(_entity->GetName().c_str());

		GameEntity** entityPtr = &_entity;
		ImGui::SetDragDropPayload("EntityData", entityPtr, sizeof(GameEntity**));

		ImGui::EndDragDropSource();
	}


	/// ---------------------------------------------------
	/// ドロップの処理
	/// ---------------------------------------------------
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityData")) {
			GameEntity** srcEntityPtr = static_cast<GameEntity**>(payload->Data);
			GameEntity* srcEntity = *srcEntityPtr;

			/// drop先とdrop元が同じでなければ親子関係を設定
			if (srcEntity != _entity) {
				/// _entityがsrcEntityの子孫である場合、無限ループになるので注意
				if (!IsDescendant(srcEntity, _entity)) {

					// 親子関係の設定
					srcEntity->RemoveParent();
					srcEntity->SetParent(_entity);
				} else {
					showInvalidParentPopup_ = true;
					Console::LogError("ドロップ先エンティティがドラッグ元エンティティの子であるためドロップできません");
				}
			}
		}
		ImGui::EndDragDropTarget();
	}


	/// ---------------------------------------------------
	/// 右クリックのメニュー表示の開始
	/// ---------------------------------------------------

	if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("EntityContextMenu");
	}

	if(ImGui::BeginPopup("EntityContextMenu")) {
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
				ImGuiSelection::SetSelectedObject(Guid::kInvalid, SelectionType::None);
			}
		}
		ImGui::EndPopup();
	}


	/// ---------------------------------------------------
	/// 名前変更処理
	/// ---------------------------------------------------




	/// ---------------------------------------------------
	/// 子エンティティがいるなら再帰的に表示
	/// ---------------------------------------------------

	ImGui::Indent(32.0f);
	for (auto& child : _entity->GetChildren()) {
		DrawEntity(child);
	}
	ImGui::Unindent(32.0f);


	return isSelected;
}

bool ImGuiHierarchyWindow::IsDescendant(GameEntity* _ancestor, GameEntity* _descendant) {
	/// ----- _ancestorが_descendantの子ではないかチェック ----- ///

	if (!_descendant) {
		return false;
	}

	GameEntity* current = _descendant->GetParent();
	while (current) {
		if (current == _ancestor) {
			return true;
		}
		current = current->GetParent();
	}

	return false;
}

void ImGuiHierarchyWindow::ShowInvalidParentPopup() {
	if (showInvalidParentPopup_) {
		ImGui::OpenPopup("Invalid Parent");

		if (ImGui::BeginPopupModal("Invalid Parent", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Cannot set a descendant as a parent!");
			if (ImGui::Button("OK")) {
				ImGui::CloseCurrentPopup();
				showInvalidParentPopup_ = false;
			}
			ImGui::EndPopup();
		}
	}
}


/// /////////////////////////////////////////////////////////////////////////
/// ImGuiNormalHierarchyWindow
/// /////////////////////////////////////////////////////////////////////////

ImGuiNormalHierarchyWindow::ImGuiNormalHierarchyWindow(const std::string& _imGuiWindowName, EntityComponentSystem* _ecs, EditorManager* _editorManager, SceneManager* _sceneManager)
	: ImGuiHierarchyWindow(_imGuiWindowName, nullptr, _editorManager, _sceneManager) {
	pEcs_ = _ecs;
}

void ImGuiNormalHierarchyWindow::ShowImGui() {
	if (!ImGui::Begin(imGuiWindowName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	/// beginで生成されたウィンドウのアクティブ状態をチェック
	if (ImGui::IsWindowFocused()) {
		/// エンティティのペーストコマンドの実行
		if (Input::PressKey(DIK_LCONTROL) || Input::PressKey(DIK_RCONTROL)) {
			if (Input::TriggerKey(DIK_V)) {
				EditCommand::Execute<PasteEntityCommand>(pECSGroup_);
			}
		}
	}

	pECSGroup_ = pEcs_->GetCurrentGroup();

	/// Prefabのドラッグ＆ドロップ
	PrefabDragAndDrop();

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("+")) {
			DrawMenuEntity();
			DrawMenuScene();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	DrawSceneDialog();
	DrawSceneSaveDialog();


	/// ヒエラルキーの表示
	DrawHierarchy();

	ImGui::End();

}

void ImGuiNormalHierarchyWindow::DrawSceneDialog() {
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
			pSceneManager_->LoadScene(filePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}
