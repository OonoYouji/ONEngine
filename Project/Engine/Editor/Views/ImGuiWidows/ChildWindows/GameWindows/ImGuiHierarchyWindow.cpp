#include "ImGuiHierarchyWindow.h"


/// external
#include <imgui.h>
#include <dialog/ImGuiFileDialog.h>
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Config/EngineConfig.h"
#include "Engine/Core/Utility/Math/Math.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"
#include "Engine/Scene/SceneManager.h"

/// editor
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"
#include "Engine/Editor/Manager/EditCommand.h"
#include "Engine/Editor/Manager/EditorManager.h"
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Views/ImGuiSelection.h"
#include "ImGuiInspectorWindow.h"

using namespace Editor;

HierarchyWindow::HierarchyWindow(
	const std::string& _imGuiWindowName,
	ONEngine::ECSGroup* _ecsGroup,
	EditorManager* _editorManager,
	ONEngine::SceneManager* _sceneManager)
	: imGuiWindowName_(_imGuiWindowName), pEcsGroup_(_ecsGroup), pEditorManager_(_editorManager),
	pSceneManager_(_sceneManager) {

	newName_.reserve(1024);
	isNodeOpen_ = false;
}

void HierarchyWindow::ShowImGui() {
	if (!ImGui::Begin(imGuiWindowName_.c_str(), nullptr)) {
		ImGui::End();
		return;
	}

	/// ヒエラルキーの表示
	DrawHierarchy();

	ImGui::End();
}

void HierarchyWindow::PrefabDragAndDrop() {
	/// ----- Prefabのドラッグアンドドロップ処理 ----- ///

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

					pEcsGroup_->GenerateEntityFromPrefab(str, ONEngine::DebugConfig::isDebugging);
					ONEngine::Console::Log(std::format("entity name set to: {}", str));
				} else {
					ONEngine::Console::Log("[error] Invalid entity format. Please use \".prefab\"");
				}
			}
		}

		ImGui::EndDragDropTarget();
	}
}

void HierarchyWindow::DrawMenuBar() {
	/// ----- MenuBarの表示 ----- ///

	/// 新規作成やシーンの保存などなど
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("+")) {
			DrawMenuEntity();
			DrawMenuScene();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	/// シーンの保存、読み込みに使うダイアログの表示
	DrawDialog();
	DrawSceneSaveDialog();
}

void HierarchyWindow::DrawMenuEntity() {
	if (ImGui::BeginMenu("create")) {
		if (ImGui::MenuItem("create empty object")) {
			pEditorManager_->ExecuteCommand<CreateGameObjectCommand>(pEcsGroup_);
		}

		ImGui::EndMenu();
	}
}

void HierarchyWindow::DrawMenuScene() {
	/// ----- sceneメニューの表示 ----- ///

	if (ImGui::BeginMenu("scene")) {

		if (ImGui::MenuItem("create scene")) {
			/// 新規のシーンファイルを作成する
			IGFD::FileDialogConfig config;
			config.path = "./Assets/Scene";
			ImGuiFileDialog::Instance()->OpenDialog("save file dialog", "ファイル保存", ".json", config);
		}

		if (ImGui::MenuItem("save scene")) {
			pSceneManager_->SaveScene(pEcsGroup_->GetGroupName(), pEcsGroup_);
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

void HierarchyWindow::DrawHierarchy() {

	/// ECSGroupないにあるEntityの表示
	if (ImGui::CollapsingHeader(pEcsGroup_->GetGroupName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		/// ---------------------------------------------------
		/// 透明なボタンを表示し、ここにエンティティがドロップされたら親子関係を解除する
		/// ---------------------------------------------------
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		windowSize.y = 12.0f;
		if (windowSize.x == 0.0f) {
			windowSize.x = 12.0f;
		}

		ImGui::InvisibleButton("HierarchyDropArea", windowSize);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityData")) {
				ONEngine::GameEntity** srcEntityPtr = static_cast<ONEngine::GameEntity**>(payload->Data);
				ONEngine::GameEntity* srcEntity = *srcEntityPtr;
				/// 親子関係の解除
				srcEntity->RemoveParent();
			}
			ImGui::EndDragDropTarget();
		}


		/// ---------------------------------------------------
		/// エンティティの表示
		/// ---------------------------------------------------

		std::vector<ONEngine::GameEntity*> entityPtrs;
		for (auto& entity : pEcsGroup_->GetEntities()) {
			/// 子を再帰的に処理するので親がないエンティティだけ処理する
			if (!entity->GetParent()) {
				entityPtrs.push_back(entity.get());
			}
		}

		for (auto& entity : entityPtrs) {
			DrawEntity(entity);
		}


		/// 無効な親子関係のポップアップ表示
		ShowInvalidParentPopup();

	}


}


void HierarchyWindow::EntityRename(ONEngine::GameEntity* _entity) {

	if (ImGuiInputText("##rename", &newName_, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_EnterReturnsTrue)) {
		pEditorManager_->ExecuteCommand<EntityRenameCommand>(_entity, newName_);
		renameEntity_ = nullptr;
	}

	// フォーカスが外れたらリネームキャンセル
	if (ONEngine::Input::TriggerMouse(ONEngine::Mouse::Right) || ONEngine::Input::TriggerKey(DIK_ESCAPE)) {
		renameEntity_ = nullptr;
	}
}

void HierarchyWindow::DrawDialog() {
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
			pEcsGroup_->RemoveEntityAll();
			pSceneManager_->GetSceneIO()->Input(filePathName, pEcsGroup_);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

}

void HierarchyWindow::DrawSceneSaveDialog() {
	/// ----- シーン保存ダイアログの表示 ----- ///

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
				ONEngine::Console::LogError("Failed to create file: " + filePathName);
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void HierarchyWindow::DrawEntity(ONEngine::GameEntity* _entity) {
	/// ----- Entityの表示 ----- ///

	bool hasChildren = !_entity->GetChildren().empty();
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;


	bool nodeOpen = false;

	/// ---------------------------------------------------
	/// Selectableでエンティティ名の表示
	/// ---------------------------------------------------

	/// 名前被りでエラーになるのを防ぐため、IDを付与
	ImGui::PushID(_entity->GetId());


	if (renameEntity_ && renameEntity_ == _entity) {
		/// ---------------------------------------------------
		/// 名前変更処理中の表示
		/// ---------------------------------------------------

		EntityRename(_entity);


	} else {
		/// ---------------------------------------------------
		/// 通常の表示
		/// ---------------------------------------------------


		/// 選択中のエンティティならSelectedフラグを付与
		if (_entity == selectedEntity_) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		/// 子エンティティがいない場合はLeafフラグを付与
		if (!hasChildren) {
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		/// TreeNodeでエンティティ名を表示 & Ckickで選択処理
		nodeOpen = ImGui::TreeNodeEx((void*)_entity, flags, "%s", _entity->GetName().c_str());
		if (ImGui::IsItemClicked()) {
			selectedEntity_ = _entity;
			ImGuiSelection::SetSelectedObject(_entity->GetGuid(), SelectionType::Entity);
		}

	}


	/// ---------------------------------------------------
	/// ドラッグの開始
	/// ---------------------------------------------------

	if (ImGui::BeginDragDropSource()) {
		ImGui::Text(_entity->GetName().c_str());

		ONEngine::GameEntity** entityPtr = &_entity;
		ImGui::SetDragDropPayload("EntityData", entityPtr, sizeof(ONEngine::GameEntity**));

		ImGui::EndDragDropSource();
	}


	/// ---------------------------------------------------
	/// ドロップの処理
	/// ---------------------------------------------------
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityData")) {
			ONEngine::GameEntity** srcEntityPtr = static_cast<ONEngine::GameEntity**>(payload->Data);
			ONEngine::GameEntity* srcEntity = *srcEntityPtr;

			/// drop先とdrop元が同じでなければ親子関係を設定
			if (srcEntity != _entity) {
				/// _entityがsrcEntityの子孫である場合、無限ループになるので注意
				if (!IsDescendant(srcEntity, _entity)) {
					EditCommand::Execute<ChangeEntityParentCommand>(srcEntity, _entity);
				} else {
					showInvalidParentPopup_ = true;
					ONEngine::Console::LogError("ドロップ先エンティティがドラッグ元エンティティの子であるためドロップできません");
				}
			}
		}
		ImGui::EndDragDropTarget();
	}


	/// ---------------------------------------------------
	/// 右クリックのメニュー表示と表示の開始処理
	/// ---------------------------------------------------

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("EntityContextMenu");
	}

	if (ImGui::BeginPopup("EntityContextMenu")) {

		/// 新規の生成(子に追加する
		if (ImGui::BeginMenu("create")) {

			if (ImGui::MenuItem("create empty object")) {
				static int count = 0;
				count++;
				std::string name = "NewEntity_" + std::to_string(count);
				pEditorManager_->ExecuteCommand<CreateGameObjectCommand>(pEcsGroup_, name, _entity);
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("rename")) {
			renameEntity_ = _entity;
			newName_ = _entity->GetName();
		}
		if (ImGui::MenuItem("delete")) {
			pEditorManager_->ExecuteCommand<DeleteEntityCommand>(pEcsGroup_, _entity);
			renameEntity_ = nullptr; // 名前変更モードを解除
			/// 選択中ならInspectorの選択を解除
			if (selectedEntity_ == _entity) {
				selectedEntity_ = nullptr;
				ImGuiSelection::SetSelectedObject(ONEngine::Guid::kInvalid, SelectionType::None);
			}
		}
		ImGui::EndPopup();
	}



	/// ---------------------------------------------------
	/// エンティティのコピーの処理
	/// ---------------------------------------------------
	if (_entity == selectedEntity_) {
		if (ONEngine::Input::PressKey(DIK_LCONTROL) || ONEngine::Input::PressKey(DIK_RCONTROL)) {
			if (ONEngine::Input::TriggerKey(DIK_C)) {
				EditCommand::Execute<CopyEntityCommand>(_entity);
			}
		}
	}



	ImGui::PopID();

	/// ---------------------------------------------------
	/// 子エンティティがいるなら再帰的に表示
	/// ---------------------------------------------------

	/// 子がある場合は再帰
	if (hasChildren && nodeOpen) {
		for (auto* child : _entity->GetChildren()) {
			DrawEntity(child);
		}
		ImGui::TreePop();
	}
}

bool HierarchyWindow::IsDescendant(ONEngine::GameEntity* _ancestor, ONEngine::GameEntity* _descendant) {
	/// ----- _ancestorが_descendantの子ではないかチェック ----- ///

	if (!_descendant) {
		return false;
	}

	ONEngine::GameEntity* current = _descendant->GetParent();
	while (current) {
		if (current == _ancestor) {
			return true;
		}
		current = current->GetParent();
	}

	return false;
}

void HierarchyWindow::ShowInvalidParentPopup() {
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

ImGuiNormalHierarchyWindow::ImGuiNormalHierarchyWindow(const std::string& _imGuiWindowName, ONEngine::EntityComponentSystem* _ecs, EditorManager* _editorManager, ONEngine::SceneManager* _sceneManager)
	: HierarchyWindow(_imGuiWindowName, nullptr, _editorManager, _sceneManager) {
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
		if (ONEngine::Input::PressKey(DIK_LCONTROL) || ONEngine::Input::PressKey(DIK_RCONTROL)) {
			if (ONEngine::Input::TriggerKey(DIK_V)) {
				EditCommand::Execute<PasteEntityCommand>(pEcsGroup_, selectedEntity_);
			}
		}
	}

	pEcsGroup_ = pEcs_->GetCurrentGroup();

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
