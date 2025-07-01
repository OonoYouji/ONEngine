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


	RegisterComponent<Transform>([&](IComponent* _component) { TransformDebug(static_cast<Transform*>(_component)); });
	RegisterComponent<DirectionalLight>([&](IComponent* _component) { DirectionalLightDebug(static_cast<DirectionalLight*>(_component)); });
	RegisterComponent<AudioSource>([&](IComponent* _component) { AudioSourceDebug(static_cast<AudioSource*>(_component)); });
	RegisterComponent<Variables>([&](IComponent* _component) { VariablesDebug(static_cast<Variables*>(_component)); });
	RegisterComponent<Effect>([&](IComponent* _component) { EffectDebug(static_cast<Effect*>(_component)); });
	RegisterComponent<MeshRenderer>([&](IComponent* _component) { MeshRendererDebug(static_cast<MeshRenderer*>(_component)); });
	RegisterComponent<CustomMeshRenderer>([&](IComponent* _component) { CustomMeshRendererDebug(static_cast<CustomMeshRenderer*>(_component)); });
	RegisterComponent<SpriteRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<Line2DRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<Line3DRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<ToTerrainCollider>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<Script>([&](IComponent* _component) { COMP_DEBUG::ScriptDebug(static_cast<Script*>(_component)); });
	RegisterComponent<SphereCollider>([&](IComponent* _component) { COMP_DEBUG::SphereColliderDebug(static_cast<SphereCollider*>(_component)); });
	RegisterComponent<BoxCollider>([&]([[maybe_unused]] IComponent* _component) {});


	/// 関数を登録
	inspectorFunctions_.emplace_back([]() {});
	inspectorFunctions_.emplace_back([this]() { EntityInspector(); });

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


void ImGuiInspectorWindow::EntityInspector() {
	IEntity* entity = reinterpret_cast<IEntity*>(selectedPointer_);
	if (!dynamic_cast<IEntity*>(entity)) {
		return;
	}

	/// ----------------------------
	/// 適当な編集の機能
	/// ----------------------------
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save")) {
				pEditorManager_->ExecuteCommand<EntityDataOutputCommand>(entity);
			}

			if (ImGui::MenuItem("Load")) {
				pEditorManager_->ExecuteCommand<EntityDataInputCommand>(entity);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	ImGuiInputTextReadOnly("entity name", entity->GetName());
	ImGuiInputTextReadOnly("entity id", "Entity ID: " + std::to_string(entity->GetId()));

	ImGui::Separator();
	/// ----------------------------
	/// componentのデバッグ
	/// ----------------------------
	for (auto& component : entity->GetComponents()) {
		std::string componentName = typeid(*component.second).name();
		//if (componentName == "") {
		//	continue;
		//}

		/// チェックボックスでenable/disableを切り替え
		bool enabled = component.second->enable;
		if (ImGui::Checkbox(("##" + componentName).c_str(), &enabled)) {
			component.second->enable = enabled;
		}

		ImGui::SameLine();

		componentName += "##" + std::to_string(reinterpret_cast<uintptr_t>(component.second));
		if (componentName.find("class ") == 0) {
			componentName = componentName.substr(6);
		}

		/// component debug
		ImGui::Separator();
		ImGui::SameLine();
		if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			componentDebugFuncs_[component.first](component.second);

		}
	}


	/// ----------------------------
	/// componentの追加
	/// ----------------------------
	ImGui::Separator();
	for (int i = 0; i < 4; ++i) {
		ImGui::Indent();
	}

	if (ImGui::Button("Add Component")) {
		ImGui::OpenPopup("AddComponent");
	}

	for (int i = 0; i < 4; ++i) {
		ImGui::Unindent();
	}


	/// popup window
	if (ImGui::BeginPopup("AddComponent", ImGuiWindowFlags_AlwaysVerticalScrollbar)) {

		ImVec2 buttonSize = ImVec2(128.f, 0.f);
		for (const auto& name : componentNames_) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if (ImGui::Button(name.second.c_str(), buttonSize)) {
				pEditorManager_->ExecuteCommand<AddComponentCommand>(entity, name.second);
			}

			ImGui::PopStyleColor();
		}

		ImGui::EndPopup();
	}


}
