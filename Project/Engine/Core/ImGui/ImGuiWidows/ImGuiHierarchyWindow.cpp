#include "ImGuiHierarchyWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/Entity/Collection/EntityCollection.h"


ImGuiHierarchyWindow::ImGuiHierarchyWindow(EntityCollection* _entityCollection)
	: entityCollection_(_entityCollection) {}

void ImGuiHierarchyWindow::ImGuiFunc() {
	ImGui::Begin("Hierarchy");

	std::string entityName = "empty";
	const std::string kClassPrefix = "class ";

	/// entityの表示
	for (auto& entity : entityCollection_->GetEntities()) {
		entityName = typeid(*entity).name();
		if (entityName.find(kClassPrefix) == 0) {
			entityName = entityName.substr(kClassPrefix.length());
		}

		ImGui::Text(entityName.c_str());
	}

	ImGui::End();
}
