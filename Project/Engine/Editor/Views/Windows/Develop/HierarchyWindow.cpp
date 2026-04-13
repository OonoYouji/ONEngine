#include "HierarchyWindow.h"

/// std
#include <filesystem>

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
#include "Engine/Editor/Math/ImGuiSelection.h"
#include "Engine/Editor/Commands/ImGuiCommand/FocusEntityCommand.h"

namespace Editor {

HierarchyWindow::HierarchyWindow(
	const std::string& windowName,
	ONEngine::EntityComponentSystem* ecs,
	ONEngine::ECSGroup* ecsGroup,
	EditorManager* editorManager,
	ONEngine::SceneManager* sceneManager)
	: windowName_(windowName), pEcs_(ecs), pEcsGroup_(ecsGroup), pEditorManager_(editorManager),
	pSceneManager_(sceneManager) {

	newName_.reserve(1024);
	isNodeOpen_ = false;

	// 無効なGuidで初期化しておく
	renameEntityGuid_ = ONEngine::Guid::kInvalid;
}

void HierarchyWindow::ShowImGui() {
	if(!ImGui::Begin(windowName_.c_str(), nullptr)) {
		ImGui::End();
		return;
	}

	/// ヒエラルキーの表示
	DrawHierarchy();

	ImGui::End();
}

void HierarchyWindow::PrefabDragAndDrop() {
	/// ----- Prefabのドラッグアンドドロップ処理 ----- ///

	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetData")) {
			if(payload->Data) {
				const char* droppedPath = static_cast<const char*>(payload->Data);
				std::string path = std::string(droppedPath);

				if(path.find(".prefab") != std::string::npos) {
					// filesystemを使って安全かつシンプルにファイル名を抽出
					std::string fileName = std::filesystem::path(path).filename().string();

					pEcsGroup_->GenerateEntityFromPrefab(fileName, ONEngine::DebugConfig::isDebugging);
					ONEngine::Console::Log(std::format("entity name set to: {}", fileName));
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

	if(ImGui::BeginMenuBar()) {
		if(ImGui::BeginMenu("+")) {
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
	if(ImGui::BeginMenu("create")) {
		if(ImGui::MenuItem("create empty object")) {
			pEditorManager_->ExecuteCommand<CreateGameObjectCommand>(pEcsGroup_);
		}
		ImGui::EndMenu();
	}
}

void HierarchyWindow::DrawMenuScene() {
	/// ----- sceneメニューの表示 ----- ///
	if(ImGui::BeginMenu("scene")) {
		if(ImGui::MenuItem("create scene")) {
			IGFD::FileDialogConfig config;
			config.path = "./Assets/Scene";
			ImGuiFileDialog::Instance()->OpenDialog("save file dialog", "ファイル保存", ".json", config);
		}

		if(ImGui::MenuItem("save scene")) {
			pSceneManager_->SaveScene(pEcsGroup_->GetGroupName(), pEcsGroup_);
		}

		if(ImGui::BeginMenu("load scene")) {
			if(ImGui::MenuItem("open explorer")) {
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
	if(ImGui::CollapsingHeader(pEcsGroup_->GetGroupName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

		HandleRootDragDrop();

		for(auto& entity : pEcsGroup_->GetEntities()) {
			if(!entity->GetParent()) {
				DrawEntity(entity.get());
			}
		}

		ShowInvalidParentPopup();
	}
}

void HierarchyWindow::EntityRename(ONEngine::GameEntity* entity) {
	if(ImGuiInputText("##rename", &newName_, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_EnterReturnsTrue)) {
		pEditorManager_->ExecuteCommand<EntityRenameCommand>(entity, newName_);
		renameEntityGuid_ = ONEngine::Guid::kInvalid; // 完了したらリセット
	}

	// フォーカスが外れたらリネームキャンセル
	if(ONEngine::Input::TriggerMouse(ONEngine::Mouse::Right) || ONEngine::Input::TriggerKey(DIK_ESCAPE)) {
		renameEntityGuid_ = ONEngine::Guid::kInvalid;
	}
}

void HierarchyWindow::DrawDialog() {
	if(ImGuiFileDialog::Instance()->Display("Dialog", ImGuiWindowFlags_NoDocking)) {
		if(ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			// filesystemを使用して、ディレクトリパスと拡張子を除外したファイル名だけを取得
			std::string sceneName = std::filesystem::path(filePathName).stem().string();

			pEcsGroup_->RemoveEntityAll();
			pSceneManager_->GetSceneIO()->Input(sceneName, pEcsGroup_);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void HierarchyWindow::DrawSceneSaveDialog() {
	if(ImGuiFileDialog::Instance()->Display("save file dialog")) {
		if(ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			nlohmann::json j = nlohmann::json::object();
			std::ofstream ofs(filePathName, std::ios::out | std::ios::binary);

			if(ofs) {
				ofs << j.dump(4);
				ofs.close();
			} else {
				ONEngine::Console::LogError("Failed to create file: " + filePathName);
			}
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

bool HierarchyWindow::IsDescendant(ONEngine::GameEntity* ancestor, ONEngine::GameEntity* descendant) {
	if(!descendant) {
		return false;
	}

	ONEngine::GameEntity* current = descendant->GetParent();
	while(current) {
		if(current == ancestor) {
			return true;
		}
		current = current->GetParent();
	}
	return false;
}

void HierarchyWindow::ShowInvalidParentPopup() {
	if(showInvalidParentPopup_) {
		ImGui::OpenPopup("Invalid Parent");

		if(ImGui::BeginPopupModal("Invalid Parent", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Cannot set a descendant as a parent!");
			if(ImGui::Button("OK")) {
				ImGui::CloseCurrentPopup();
				showInvalidParentPopup_ = false;
			}
			ImGui::EndPopup();
		}
	}
}



///	-------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// エンティティのエディタ表示
///	-------------------------------------------------------------------------------------------------------------------------------------------------------------------


///
/// エンティティのエディタ表示
/// 
void HierarchyWindow::DrawEntity(ONEngine::GameEntity* entity) {
	bool hasChildren = !entity->GetChildren().empty();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	ImGui::PushID(entity->GetId());

	bool isSelected = (ImGuiSelection::GetSelectedObject().CheckValid() && ImGuiSelection::GetSelectedObject() == entity->GetGuid());

	if(isSelected) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if(!hasChildren) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	/// ===================================================
	/// 1. 開閉ボタン（矢印）と行ベースの描画
	/// ===================================================
	// ラベルを空文字列 ("") にして、矢印と背景の選択ハイライトだけを描画させる
	bool nodeOpen = ImGui::TreeNodeEx((void*)entity, flags, "");

	HandleEntityDragDrop(entity);
	DrawEntityContextMenu(entity, isSelected);

	// 行がホバーされているときの処理
	if(ImGui::IsItemHovered()) {
		// クリックで選択
		if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
			ImGuiSelection::SetSelectedObject(entity->GetGuid(), SelectionType::Entity);
		}

		// ダブルクリックでフォーカス
		if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			EditCommand::Execute<FocusEntityCommand>(pEcs_, entity);
		}
	}

	/// ===================================================
	/// 2. アクティブ / 非アクティブ のチェックボックス
	/// ===================================================
	ImGui::SameLine();

	bool isActive = entity->active;
	if(ImGui::Checkbox("##Active", &isActive)) {
		entity->active = isActive;
	}

	/// ===================================================
	/// 3. 名前の表示 または リネーム入力欄
	/// ===================================================
	ImGui::SameLine();

	if(renameEntityGuid_.CheckValid() && renameEntityGuid_ == entity->GetGuid()) {
		EntityRename(entity);
	} else {
		// 通常の名前表示
		ImGui::Text("%s", entity->GetName().c_str());

		// テキスト部分がホバーされているときの処理
		if(ImGui::IsItemHovered()) {
			// クリックで選択（直感的なUXのため）
			if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				ImGuiSelection::SetSelectedObject(entity->GetGuid(), SelectionType::Entity);
			}
			// ★追加：テキスト部分のダブルクリックでフォーカス
			if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				// 例: pEditorManager_->ExecuteCommand<FocusEntityCommand>(entity);
				// （Fキーを押した際と同じ関数やコマンドをここに記述してください）
			}
		}
	}

	/// ===================================================
	/// 4. ショートカット処理の呼び出し
	/// ===================================================
	HandleEntityShortcuts(entity, isSelected);

	ImGui::PopID();

	/// 子エンティティの再帰的描画
	if(hasChildren && nodeOpen) {
		for(auto* child : entity->GetChildren()) {
			DrawEntity(child);
		}
		ImGui::TreePop();
	}
}


///
/// 親子関係の解除/ルートへの移動
///
void HierarchyWindow::HandleRootDragDrop() {
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	windowSize.y = 12.0f;
	if(windowSize.x == 0.0f) {
		windowSize.x = 12.0f;
	}

	// 階層の隙間に透明な判定エリアを作る
	ImGui::InvisibleButton("HierarchyDropArea", windowSize);
	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityData")) {
			ONEngine::GameEntity** srcEntityPtr = static_cast<ONEngine::GameEntity**>(payload->Data);
			ONEngine::GameEntity* srcEntity = *srcEntityPtr;
			srcEntity->RemoveParent();
		}
		ImGui::EndDragDropTarget();
	}
}


///
/// エンティティ同士の親子関係の構築
///
void HierarchyWindow::HandleEntityDragDrop(ONEngine::GameEntity* entity) {
	// ドラッグ開始（Source）
	if(ImGui::BeginDragDropSource()) {
		ImGui::Text(entity->GetName().c_str());
		ONEngine::GameEntity** entityPtr = &entity;
		ImGui::SetDragDropPayload("EntityData", entityPtr, sizeof(ONEngine::GameEntity**));
		ImGui::EndDragDropSource();
	}

	// ドロップ受け入れ（Target）
	if(ImGui::BeginDragDropTarget()) {
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntityData")) {
			ONEngine::GameEntity** srcEntityPtr = static_cast<ONEngine::GameEntity**>(payload->Data);
			ONEngine::GameEntity* srcEntity = *srcEntityPtr;

			// 自分自身や、自分の子孫ノードを親にしようとしていないかチェック
			if(srcEntity != entity) {
				if(!IsDescendant(srcEntity, entity)) {
					EditCommand::Execute<ChangeEntityParentCommand>(srcEntity, entity);
				} else {
					showInvalidParentPopup_ = true;
					ONEngine::Console::LogError("ドロップ先エンティティがドラッグ元エンティティの子であるためドロップできません");
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

///
/// エンティティの右クリックメニューの処理
///
void HierarchyWindow::DrawEntityContextMenu(ONEngine::GameEntity* entity, bool selected) {
	if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("EntityContextMenu");
	}

	if(ImGui::BeginPopup("EntityContextMenu")) {
		if(ImGui::BeginMenu("create")) {
			if(ImGui::MenuItem("create empty object")) {
				static int count = 0;
				count++;
				std::string name = "NewEntity_" + std::to_string(count);
				pEditorManager_->ExecuteCommand<CreateGameObjectCommand>(pEcsGroup_, name, entity);
			}
			ImGui::EndMenu();
		}

		if(ImGui::MenuItem("rename")) {
			renameEntityGuid_ = entity->GetGuid();
			newName_ = entity->GetName();
		}

		if(ImGui::MenuItem("delete")) {
			pEditorManager_->ExecuteCommand<DeleteEntityCommand>(pEcsGroup_, entity);
			renameEntityGuid_ = ONEngine::Guid::kInvalid;

			// 削除したエンティティが選択中だった場合は、選択状態も解除する
			if(selected) {
				ImGuiSelection::SetSelectedObject(ONEngine::Guid::kInvalid, SelectionType::None);
			}
		}
		ImGui::EndPopup();
	}
}

///
/// ノードごとのショートカット入力
///
void HierarchyWindow::HandleEntityShortcuts(ONEngine::GameEntity* entity, bool selected) {
	if(selected) {
		if(ONEngine::Input::PressKey(DIK_LCONTROL) || ONEngine::Input::PressKey(DIK_RCONTROL)) {
			if(ONEngine::Input::TriggerKey(DIK_C)) {
				EditCommand::Execute<CopyEntityCommand>(entity);
			}
		}
	}
}


/// /////////////////////////////////////////////////////////////////////////
/// ImGuiNormalHierarchyWindow
/// /////////////////////////////////////////////////////////////////////////

NormalHierarchyWindow::NormalHierarchyWindow(const std::string& windowName, ONEngine::EntityComponentSystem* _ecs, EditorManager* editorManager, ONEngine::SceneManager* sceneManager)
	: HierarchyWindow(windowName, _ecs, nullptr, editorManager, sceneManager) {
	pEcs_ = _ecs;
}

void NormalHierarchyWindow::ShowImGui() {
	if(!ImGui::Begin(windowName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	HandleGlobalShortcuts();

	pEcsGroup_ = pEcs_->GetCurrentGroup();
	PrefabDragAndDrop();

	if(ImGui::BeginMenuBar()) {
		if(ImGui::BeginMenu("+")) {
			DrawMenuEntity();
			DrawMenuScene();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	DrawSceneDialog();
	DrawSceneSaveDialog();
	DrawHierarchy();

	ImGui::End();
}

void NormalHierarchyWindow::DrawSceneDialog() {
	if(ImGuiFileDialog::Instance()->Display("Dialog", ImGuiWindowFlags_NoDocking)) {
		if(ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			// filesystemで簡潔に拡張子を除去
			std::string sceneName = std::filesystem::path(filePathName).stem().string();

			pSceneManager_->LoadScene(sceneName);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void NormalHierarchyWindow::HandleGlobalShortcuts() {
	if(ImGui::IsWindowFocused()) {
		if(ONEngine::Input::PressKey(DIK_LCONTROL) || ONEngine::Input::PressKey(DIK_RCONTROL)) {
			if(ONEngine::Input::TriggerKey(DIK_V)) {
				ONEngine::Guid selectedGuid = ImGuiSelection::GetSelectedObject();
				if(selectedGuid.CheckValid()) {
					ONEngine::GameEntity* targetEntity = pEcsGroup_->GetEntityFromGuid(selectedGuid);
					if(targetEntity) {
						EditCommand::Execute<PasteEntityCommand>(pEcsGroup_, targetEntity);
					}
				} else {
					EditCommand::Execute<PasteEntityCommand>(pEcsGroup_, nullptr);
				}
			}
		}
	}
}

} /// namespace Editor