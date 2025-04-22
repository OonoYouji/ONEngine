#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "ImGuiInspectorWindow.h"


ImGuiHierarchyWindow::ImGuiHierarchyWindow(EntityComponentSystem* _pEntityComponentSystem, ImGuiInspectorWindow* _inspectorWindow)
	: pEntityComponentSystem_(_pEntityComponentSystem), pInspectorWindow_(_inspectorWindow) {}

void ImGuiHierarchyWindow::ImGuiFunc() {
	if (!ImGui::Begin("Hierarchy", nullptr, 0)) {
		ImGui::End();
		return;
	}


	/// entityの選択
	entityList_.clear();
	for (auto& entity : pEntityComponentSystem_->GetEntities()) {
		if (!entity->GetParent()) { //!< 親がいない場合
			entityList_.push_back(entity.get());
		}
	}


	/// entityの表示
	for (auto& entity : entityList_) {
		entityName_ = typeid(*entity).name();
		entityName_ += "##" + std::to_string(reinterpret_cast<uintptr_t>(entity));
		if (entityName_.find(kClassPrefix) == 0) {
			entityName_ = entityName_.substr(kClassPrefix.length());
		}

		if (ImGui::Selectable(entityName_.c_str(), entity == selectedEntity_)) {
			selectedEntity_ = entity;
		}

		DrawChildren(entity);  // 子要素を描画
	}


	pInspectorWindow_->SetSelectedEntity(reinterpret_cast<std::uintptr_t>(selectedEntity_));

	ImGui::End();
}

void ImGuiHierarchyWindow::DrawChildren(IEntity* _entity) {
	for (auto& child : _entity->GetChildren()) {
		entityName_ = typeid(*child).name();
		entityName_ += "##" + std::to_string(reinterpret_cast<uintptr_t>(child));
		if (entityName_.find(kClassPrefix) == 0) {
			entityName_ = entityName_.substr(kClassPrefix.length());
		}

		ImGui::Indent();
		if (child->GetChildren().empty()) {
			// 子がいない場合はSelectableで選択可能にする  
			if (ImGui::Selectable(entityName_.c_str(), child == selectedEntity_)) {
				selectedEntity_ = child;
			}
		} else {
			// 子がいる場合はTreeNodeを使用して階層構造を開閉可能にする  
			if (ImGui::TreeNodeEx(entityName_.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth)) {
				if (ImGui::Selectable(entityName_.c_str(), child == selectedEntity_)) {
					selectedEntity_ = child;
				}
				DrawChildren(child);
				ImGui::TreePop();
			}
		}

		ImGui::Unindent();
	}
}
