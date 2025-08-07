#include "ImGuiInspectorWindow.h"

/// std
#include <format>

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "../../../Math/ImGuiMath.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"


enum SelectedType {
	kNone,
	kEntity,
	kResource
};

ImGuiInspectorWindow::ImGuiInspectorWindow(EntityComponentSystem* _ecs, EditorManager* _editorManager)
	: pECS_(_ecs), pEditorManager_(_editorManager) {


	/// compute
	RegisterComponent<Transform>([&](IComponent* _component) { COMP_DEBUG::TransformDebug(static_cast<Transform*>(_component)); });
	RegisterComponent<DirectionalLight>([&](IComponent* _component) { DirectionalLightDebug(static_cast<DirectionalLight*>(_component)); });
	RegisterComponent<AudioSource>([&](IComponent* _component) { AudioSourceDebug(static_cast<AudioSource*>(_component)); });
	RegisterComponent<Variables>([&](IComponent* _component) { COMP_DEBUG::VariablesDebug(static_cast<Variables*>(_component)); });
	RegisterComponent<Effect>([&](IComponent* _component) { COMP_DEBUG::EffectDebug(static_cast<Effect*>(_component)); });
	RegisterComponent<Script>([&](IComponent* _component) { COMP_DEBUG::ScriptDebug(static_cast<Script*>(_component)); });
	RegisterComponent<Terrain>([&](IComponent* _component) { COMP_DEBUG::TerrainDebug(static_cast<Terrain*>(_component)); });
	RegisterComponent<TerrainCollider>([&](IComponent* _component) { COMP_DEBUG::TerrainColliderDebug(static_cast<TerrainCollider*>(_component)); });
	RegisterComponent<CameraComponent>([&](IComponent* _component) { COMP_DEBUG::CameraDebug(static_cast<CameraComponent*>(_component)); });

	/// renderer
	RegisterComponent<MeshRenderer>([&](IComponent* _component) { COMP_DEBUG::MeshRendererDebug(static_cast<MeshRenderer*>(_component)); });
	RegisterComponent<CustomMeshRenderer>([&](IComponent* _component) { CustomMeshRendererDebug(static_cast<CustomMeshRenderer*>(_component)); });
	RegisterComponent<SpriteRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<Line2DRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<Line3DRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<SkinMeshRenderer>([&](IComponent* _component) { COMP_DEBUG::SkinMeshRendererDebug(static_cast<SkinMeshRenderer*>(_component)); });
	RegisterComponent<ScreenPostEffectTag>([&](IComponent* _component) { COMP_DEBUG::ScreenPostEffectTagDebug(static_cast<ScreenPostEffectTag*>(_component)); });
	RegisterComponent<Skybox>([&](IComponent* _component) { COMP_DEBUG::SkyboxDebug(static_cast<Skybox*>(_component)); });

	/// collider
	RegisterComponent<SphereCollider>([&](IComponent* _component) { COMP_DEBUG::SphereColliderDebug(static_cast<SphereCollider*>(_component)); });
	RegisterComponent<BoxCollider>([&](IComponent* _component) { COMP_DEBUG::BoxColliderDebug(static_cast<BoxCollider*>(_component)); });


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
	uint64_t pointerValue = reinterpret_cast<uint64_t>(entity->GetTransform());
	if (pointerValue == 0xdddddddddddddddd) {
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

		if (ImGui::MenuItem("Apply Prefab")) {

			if (!entity->GetPrefabName().empty()) {
				pEditorManager_->ExecuteCommand<CreatePrefabCommand>(entity);
				pECS_->ReloadPrefab(entity->GetPrefabName()); // Prefabを再読み込み
				//pEditorManager_->ExecuteCommand<ApplyPrefabCommand>(entity);
			} else {
				Console::LogError("This entity is not a prefab instance.");
			}

		}

		ImGui::EndMenuBar();
	}

	if (!entity->GetPrefabName().empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0, 0, 1));
		ImGuiInputTextReadOnly("entity prefab name", entity->GetPrefabName());
		ImGui::PopStyleColor();
	}

	ImGuiInputTextReadOnly("entity name", entity->GetName());
	ImGuiInputTextReadOnly("entity id", "Entity ID: " + std::to_string(entity->GetId()));

	ImGui::Separator();
	/// ----------------------------
	/// componentのデバッグ
	/// ----------------------------
	for (auto itr = entity->GetComponents().begin(); itr != entity->GetComponents().end(); ) {
		std::pair<size_t, IComponent*> component = *itr;
		std::string componentName = typeid(*component.second).name();
		if (componentName.find("class ") == 0) {
			componentName = componentName.substr(6);
		}

		std::string label = componentName + "##" + std::to_string(reinterpret_cast<uintptr_t>(component.second));


		/// チェックボックスでenable/disableを切り替え
		bool enabled = component.second->enable;
		if (ImGui::Checkbox(("##" + label).c_str(), &enabled)) {
			component.second->enable = enabled;
		}

		ImGui::SameLine();



		/// アクティブ/非アクティブで表示を変える
		if (!enabled) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0.75f, 0.75f, 1.0f)); // 白色
		}

		/// component debug
		ImGui::Separator();
		ImGui::SameLine();
		if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			/// 右クリックでポップアップメニューを開く
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup(label.c_str());
			}

			ImGui::Indent(34.0f);
			if (componentDebugFuncs_.contains(component.first)) {
				componentDebugFuncs_[component.first](component.second);
			}
			ImGui::Unindent(34.0f);
		}

		if (!enabled) {
			ImGui::PopStyleColor();
		}


		if (ImGui::BeginPopupContextItem(label.c_str())) {
			if (ImGui::MenuItem("delete")) {
				auto resultItr = entity->GetComponents().begin();
				pEditorManager_->ExecuteCommand<RemoveComponentCommand>(entity, componentName, &resultItr);
				itr = resultItr; // イテレータを更新

				/// endじゃないかチェック
				if (itr == entity->GetComponents().end()) {
					ImGui::EndPopup();
					break; // もしendに到達したらループを抜ける
				}

			}

			if (ImGui::MenuItem("reload")) {
				//pEditorManager_->ExecuteCommand<ReloadComponentCommand>(entity, componentName);
			}

			ImGui::EndPopup();
		}


		++itr;

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

IEntity* ImGuiInspectorWindow::GetSelectedEntity() const {
	if (selectedPointer_ == 0) {
		return nullptr;
	}

	IEntity* entity = reinterpret_cast<IEntity*>(selectedPointer_);
	if (dynamic_cast<IEntity*>(entity)) {
		return entity;
	}

	return nullptr;
}
