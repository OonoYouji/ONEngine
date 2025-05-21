#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "ImGuiInspectorWindow.h"


ImGuiHierarchyWindow::ImGuiHierarchyWindow(EntityComponentSystem* _pEntityComponentSystem, ImGuiInspectorWindow* _inspectorWindow)
	: pEntityComponentSystem_(_pEntityComponentSystem), pInspectorWindow_(_inspectorWindow) {}

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
	entityName_ = typeid(*_entity).name();
	entityName_ += "##" + std::to_string(reinterpret_cast<uintptr_t>(_entity));
	if (entityName_.find(kClassPrefix) == 0) {
		entityName_ = entityName_.substr(kClassPrefix.length());
	}

	if (_entity->GetChildren().empty()) {
		// 子がいない場合はSelectableで選択可能にする  
		if (ImGui::Selectable(entityName_.c_str(), _entity == selectedEntity_)) {
			selectedEntity_ = _entity;
		}
	} else {
		// 子がいる場合はTreeNodeを使用して階層構造を開閉可能にする  
		bool nodeOpen = ImGui::TreeNodeEx(entityName_.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth);
		if (ImGui::IsItemClicked()) {
			selectedEntity_ = _entity;
		}

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

//void ImGuiHierarchyWindow::DrawChildren(IEntity* _entity) {
//	for (auto& child : _entity->GetChildren()) {
//		entityName_ = typeid(*child).name();
//		entityName_ += "##" + std::to_string(reinterpret_cast<uintptr_t>(child));
//		if (entityName_.find(kClassPrefix) == 0) {
//			entityName_ = entityName_.substr(kClassPrefix.length());
//		}
//
//		ImGui::Indent();
//		if (child->GetChildren().empty()) {
//			// 子がいない場合はSelectableで選択可能にする  
//			if (ImGui::Selectable(entityName_.c_str(), child == selectedEntity_)) {
//				selectedEntity_ = child;
//			}
//		} else {
//			// 子がいる場合はTreeNodeを使用して階層構造を開閉可能にする  
//			if (ImGui::TreeNodeEx(entityName_.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth)) {
//				if (ImGui::Selectable(entityName_.c_str(), child == selectedEntity_)) {
//					selectedEntity_ = child;
//				}
//				DrawChildren(child);
//				ImGui::TreePop();
//			}
//		}
//
//		ImGui::Unindent();
//	}
//}
