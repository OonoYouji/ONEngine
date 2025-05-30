#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/EditorManager.h"
#include "ImGuiInspectorWindow.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"


ImGuiHierarchyWindow::ImGuiHierarchyWindow(EntityComponentSystem* _pEntityComponentSystem, EditorManager* _editorManager, ImGuiInspectorWindow* _inspectorWindow)
	: pEntityComponentSystem_(_pEntityComponentSystem), pEditorManager_(_editorManager), pInspectorWindow_(_inspectorWindow) {

	newName_.reserve(1024);
}

void ImGuiHierarchyWindow::ImGuiFunc() {
	if (!ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	MenuBar();

	/// ヒエラルキーの表示
	Hierarchy();

	ImGui::End();
}

void ImGuiHierarchyWindow::DrawEntityHierarchy(IEntity* _entity) {
	entityName_ = _entity->GetName();
	entityName_ += "##" + std::to_string(reinterpret_cast<uintptr_t>(_entity));

	/// 現在選択しているエンティティと違う場合はリネームをキャンセルする
	if (renameEntity_ != _entity) {
		//renameEntity_ = nullptr;
	}


	auto PopupWindow = [&]() {

		// ドラッグの開始
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("ENTITY_HIERARCHY", _entity, sizeof(IEntity));
			ImGui::TextUnformatted(entityName_.c_str());
			ImGui::EndDragDropSource();
		}

		/// 右クリックしたときのメニューの表示
		if (ImGui::BeginPopupContextItem(entityName_.c_str())) {
			if (ImGui::MenuItem("rename")) {
				newName_ = _entity->GetName();
				renameEntity_ = _entity;
				//pEditorManager_->ExecuteCommand<EntityRenameCommand>(_entity, newName_);
			}
			ImGui::EndPopup();
		}
		};



	/// 子オブジェクトの表示
	if (_entity->GetChildren().empty()) {
		if (renameEntity_ && renameEntity_ == _entity) {

			if (ImGuiInputText("##rename", &newName_, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_EnterReturnsTrue)) {
				pEditorManager_->ExecuteCommand<EntityRenameCommand>(_entity, newName_);
				renameEntity_ = nullptr;
			}

			// フォーカスが外れたらリネームキャンセル
			if (Input::TriggerMouse(Mouse::Right) || Input::TriggerKey(DIK_ESCAPE)) {
				renameEntity_ = nullptr;
			}

		} else {

			// 子がいない場合はSelectableで選択可能にする  
			if (ImGui::Selectable(entityName_.c_str(), _entity == selectedEntity_)) {
				selectedEntity_ = _entity;
			}
		}

		/// 右クリックしたときのメニューの表示
		PopupWindow();

	} else {
		// 子がいる場合はTreeNodeを使用して階層構造を開閉可能にする  
		bool nodeOpen = ImGui::TreeNodeEx(entityName_.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth);
		if (ImGui::IsItemClicked()) {
			selectedEntity_ = _entity;
		}

		/// 右クリックしたときのメニューの表示
		PopupWindow();

		if (nodeOpen) {

			ImGui::Indent();
			for (auto& child : _entity->GetChildren()) {
				DrawEntityHierarchy(child);
			}
			ImGui::Unindent();
			ImGui::TreePop();
		}
	}
}

void ImGuiHierarchyWindow::MenuBar() {

	/// 早期リターン
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	if (!ImGui::BeginMenu("+")) {
		ImGui::EndMenuBar();
		return;
	}

	/// メニューの表示
	if (ImGui::BeginMenu("create")) {
		if (ImGui::MenuItem("create empty object")) {
			pEditorManager_->ExecuteCommand<CreateGameObjectCommand>(pEntityComponentSystem_);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenu();
	ImGui::EndMenuBar();
}

void ImGuiHierarchyWindow::Hierarchy() {
	/// entityの選択  
	entityList_.clear();
	for (auto& entity : pEntityComponentSystem_->GetEntities()) {
		if (!entity->GetParent()) { //!< 親がいない場合  
			entityList_.push_back(entity.get());
		}
	}

	/// entityの表示  
	for (auto& entity : entityList_) {
		DrawEntityHierarchy(entity);
	}

	pInspectorWindow_->SetSelectedEntity(reinterpret_cast<std::uintptr_t>(selectedEntity_));
}
