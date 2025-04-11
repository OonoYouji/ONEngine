#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


ImGuiHierarchyWindow::ImGuiHierarchyWindow(EntityComponentSystem* _pEntityComponentSystem)
	: pEntityComponentSystem_(_pEntityComponentSystem) {}

void ImGuiHierarchyWindow::ImGuiFunc() {
	if (!ImGui::Begin("Hierarchy", nullptr, 0)) {
		ImGui::End();
		return;
	}

	std::string entityName = "empty";
	const std::string kClassPrefix = "class ";

	/// entityの表示
	for (auto& entity : pEntityComponentSystem_->GetEntities()) {
		entityName = typeid(*entity).name();
        entityName += "##" + std::to_string(reinterpret_cast<uintptr_t>(entity.get()));
		if (entityName.find(kClassPrefix) == 0) {
			entityName = entityName.substr(kClassPrefix.length());
		}

		if (ImGui::Selectable(entityName.c_str(), entity.get() == selectedEntity_)) {
			selectedEntity_ = entity.get();
		}
	}

	ImGui::End();
}
