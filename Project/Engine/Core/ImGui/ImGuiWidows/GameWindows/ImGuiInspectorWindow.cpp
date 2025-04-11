#include "ImGuiInspectorWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"


enum SelectedType {
	kNone,
	kEntity,
	kResource
};


ImGuiInspectorWindow::ImGuiInspectorWindow() {
	inspectorFunctions_.emplace_back([]() {});
	inspectorFunctions_.emplace_back(
		[this]() {
			//ImGui::Text("entity");

			IEntity* entity = reinterpret_cast<IEntity*>(selectedPointer_);

			for (auto& component : entity->GetComponents()) {
				std::string componentName = typeid(*component.second).name();
				componentName += "##" + std::to_string(reinterpret_cast<uintptr_t>(component.second));
				if (componentName.find("class ") == 0) {
					componentName = componentName.substr(6);
				}
				if (ImGui::Selectable(componentName.c_str(), component.second == selectedComponent_)) {
					selectedComponent_ = component.second;
				}
			}

		}
	);
}

void ImGuiInspectorWindow::ImGuiFunc() {
	if (!ImGui::Begin("Inspector")) {
		ImGui::End();
		return;
	}

	SelectedType selectedType = kNone;
	if (reinterpret_cast<IEntity*>(selectedPointer_)) {
		selectedType = kEntity;
	}

	inspectorFunctions_[selectedType]();

	ImGui::End();
}