#include "ImGuiInspectorWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"
#include "../../Math/ImGuiMath.h"

enum SelectedType {
	kNone,
	kEntity,
	kResource
};


ImGuiInspectorWindow::ImGuiInspectorWindow() {


	RegisterComponentDebugFunc(typeid(Transform).hash_code(),          [&](IComponent* _component) { TransformDebug(reinterpret_cast<Transform*>(_component)); });
	RegisterComponentDebugFunc(typeid(DirectionalLight).hash_code(),   [&](IComponent* _component) { DirectionalLightDebug(reinterpret_cast<DirectionalLight*>(_component)); });
	RegisterComponentDebugFunc(typeid(AudioSource).hash_code(),        [&](IComponent* _component) { AudioSourceDebug(reinterpret_cast<AudioSource*>(_component)); });
	RegisterComponentDebugFunc(typeid(Variables).hash_code(),          [&](IComponent* _component) { VariablesDebug(reinterpret_cast<Variables*>(_component)); });
	RegisterComponentDebugFunc(typeid(Effect).hash_code(),             [&]( [[maybe_unused]] IComponent* _component) { });
	RegisterComponentDebugFunc(typeid(MeshRenderer).hash_code(),       [&](IComponent* _component) { MeshRendererDebug(reinterpret_cast<MeshRenderer*>(_component));});
	RegisterComponentDebugFunc(typeid(CustomMeshRenderer).hash_code(), [&](IComponent* _component) { CustomMeshRendererDebug(reinterpret_cast<CustomMeshRenderer*>(_component));});
	RegisterComponentDebugFunc(typeid(SpriteRenderer).hash_code(),     [&]( [[maybe_unused]] IComponent* _component) { });
	RegisterComponentDebugFunc(typeid(Line2DRenderer).hash_code(),     [&]( [[maybe_unused]] IComponent* _component) { });
	RegisterComponentDebugFunc(typeid(Line3DRenderer).hash_code(),     [&]( [[maybe_unused]] IComponent* _component) { });
	RegisterComponentDebugFunc(typeid(CircleCollider).hash_code(),     [&]( [[maybe_unused]] IComponent* _component) { });


	inspectorFunctions_.emplace_back([]() {});
	inspectorFunctions_.emplace_back(
		[this]() {
			IEntity* entity = reinterpret_cast<IEntity*>(selectedPointer_);

			for (auto& component : entity->GetComponents()) {
				std::string componentName = typeid(*component.second).name();
				componentName += "##" + std::to_string(reinterpret_cast<uintptr_t>(component.second));
				if (componentName.find("class ") == 0) {
					componentName = componentName.substr(6);
				}

				
				/// component debug
				ImGui::Separator();
				if (ImGui::TreeNodeEx(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
					componentDebugFuncs_[component.first](component.second);

					ImGui::TreePop();
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