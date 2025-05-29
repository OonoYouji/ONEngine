#include "ImGuiInspectorWindow.h"

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"
#include "../../Math/ImGuiMath.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"


enum SelectedType {
	kNone,
	kEntity,
	kResource
};

ImGuiInspectorWindow::ImGuiInspectorWindow(EditorManager* _editorManager)
	: pEditorManager_(_editorManager) {


	RegisterComponentDebugFunc(GetComponentHash<Transform>(), [&](IComponent* _component) { TransformDebug(reinterpret_cast<Transform*>(_component)); });
	RegisterComponentDebugFunc(GetComponentHash<DirectionalLight>(), [&](IComponent* _component) { DirectionalLightDebug(reinterpret_cast<DirectionalLight*>(_component)); });
	RegisterComponentDebugFunc(GetComponentHash<AudioSource>(), [&](IComponent* _component) { AudioSourceDebug(reinterpret_cast<AudioSource*>(_component)); });
	RegisterComponentDebugFunc(GetComponentHash<Variables>(), [&](IComponent* _component) { VariablesDebug(reinterpret_cast<Variables*>(_component)); });
	RegisterComponentDebugFunc(GetComponentHash<Effect>(), [&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponentDebugFunc(GetComponentHash<MeshRenderer>(), [&](IComponent* _component) { MeshRendererDebug(reinterpret_cast<MeshRenderer*>(_component)); });
	RegisterComponentDebugFunc(GetComponentHash<CustomMeshRenderer>(), [&](IComponent* _component) { CustomMeshRendererDebug(reinterpret_cast<CustomMeshRenderer*>(_component)); });
	RegisterComponentDebugFunc(GetComponentHash<SpriteRenderer>(), [&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponentDebugFunc(GetComponentHash<Line2DRenderer>(), [&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponentDebugFunc(GetComponentHash<Line3DRenderer>(), [&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponentDebugFunc(GetComponentHash<ToTerrainCollider>(), [&]([[maybe_unused]] IComponent* _component) {});


	inspectorFunctions_.emplace_back([]() {});
	inspectorFunctions_.emplace_back(
		[this]() {
			IEntity* entity = reinterpret_cast<IEntity*>(selectedPointer_);

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Save")) {
						pEditorManager_->ExecuteCommand<EntityDataOutputCommand>(entity);
					}
				
					if (ImGui::MenuItem("Loas")) {
						pEditorManager_->ExecuteCommand<EntityDataInputCommand>(entity, "");
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}


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


			ImGui::Separator();
			for (int i = 0; i < 4; ++i) {
				ImGui::Indent();
			}

			if (ImGui::Button("Add Component")) {
				//pEditorManager_->OpenComponentAddWindow(entity);
			}
			
			for (int i = 0; i < 4; ++i) {
				ImGui::Unindent();
			}

		}
	);
}



void ImGuiInspectorWindow::ImGuiFunc() {
	if (!ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_MenuBar)) {
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