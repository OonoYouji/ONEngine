#include "ImGuiPrefabInspectorWindow.h"

/// externels
#include <imgui.h>

/// engine
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Component.h"
#include "Engine/ECS/Entity/Interface/IEntity.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"


enum SelectedType {
	kNone,
	kEntity,
	kResource
};

ImGuiPrefabInspectorWindow::ImGuiPrefabInspectorWindow(EntityComponentSystem* _ecs, EditorManager* _editorManager)
	: pECS_(_ecs), pEditorManager_(_editorManager) {

	/// compute
	RegisterComponent<Transform>([&](IComponent* _component) { COMP_DEBUG::TransformDebug(static_cast<Transform*>(_component)); });
	RegisterComponent<DirectionalLight>([&](IComponent* _component) { DirectionalLightDebug(static_cast<DirectionalLight*>(_component)); });
	RegisterComponent<AudioSource>([&](IComponent* _component) { AudioSourceDebug(static_cast<AudioSource*>(_component)); });
	RegisterComponent<Variables>([&](IComponent* _component) { VariablesDebug(static_cast<Variables*>(_component)); });
	RegisterComponent<Effect>([&](IComponent* _component) { COMP_DEBUG::EffectDebug(static_cast<Effect*>(_component)); });
	RegisterComponent<Script>([&](IComponent* _component) { COMP_DEBUG::ScriptDebug(static_cast<Script*>(_component)); });

	/// renderer
	RegisterComponent<MeshRenderer>([&](IComponent* _component) { COMP_DEBUG::MeshRendererDebug(static_cast<MeshRenderer*>(_component)); });
	RegisterComponent<CustomMeshRenderer>([&](IComponent* _component) { CustomMeshRendererDebug(static_cast<CustomMeshRenderer*>(_component)); });
	RegisterComponent<SpriteRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<Line2DRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<Line3DRenderer>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<SkinMeshRenderer>([&](IComponent* _component) { COMP_DEBUG::SkinMeshRendererDebug(static_cast<SkinMeshRenderer*>(_component)); });

	/// collider
	RegisterComponent<ToTerrainCollider>([&]([[maybe_unused]] IComponent* _component) {});
	RegisterComponent<SphereCollider>([&](IComponent* _component) { COMP_DEBUG::SphereColliderDebug(static_cast<SphereCollider*>(_component)); });
	RegisterComponent<BoxCollider>([&](IComponent* _component) { COMP_DEBUG::BoxColliderDebug(static_cast<BoxCollider*>(_component)); });


	/// 関数を登録
	inspectorFunctions_.emplace_back([]() {});
	inspectorFunctions_.emplace_back([this]() { EntityInspector(); });

}

void ImGuiPrefabInspectorWindow::ImGuiFunc() {
	if (!ImGui::Begin("prefab inspector", nullptr, ImGuiWindowFlags_MenuBar)) {
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


void ImGuiPrefabInspectorWindow::EntityInspector() {
	IEntity* entity = reinterpret_cast<IEntity*>(selectedPointer_);
	if (!dynamic_cast<IEntity*>(entity)) {
		return;
	}

	/// ----------------------------
	/// 適当な編集の機能
	/// ----------------------------
	if (ImGui::BeginMenuBar()) {
		if (ImGui::MenuItem("Apply")) {
			pEditorManager_->ExecuteCommand<CreatePrefabCommand>(entity);
			pECS_->ReloadPrefab(entity->GetPrefabName()); // Prefabを再読み込み
		}

		ImGui::EndMenuBar();
	}

	if (!entity->GetPrefabName().empty()) {
		ImGuiInputTextReadOnly("entity prefab name", entity->GetPrefabName());
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

		std::string lable = componentName + "##" + std::to_string(reinterpret_cast<uintptr_t>(component.second));


		/// チェックボックスでenable/disableを切り替え
		bool enabled = component.second->enable;
		if (ImGui::Checkbox(("##" + lable).c_str(), &enabled)) {
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
		if (ImGui::CollapsingHeader(lable.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			/// 右クリックでポップアップメニューを開く
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup(lable.c_str());
			}

			ImGui::Indent(34.0f);
			componentDebugFuncs_[component.first](component.second);
			ImGui::Unindent(34.0f);
		}

		if (!enabled) {
			ImGui::PopStyleColor();
		}


		if (ImGui::BeginPopupContextItem(lable.c_str())) {
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

IEntity* ImGuiPrefabInspectorWindow::GetSelectedEntity() const {
	IEntity* entity = reinterpret_cast<IEntity*>(selectedPointer_);
	if (dynamic_cast<IEntity*>(entity)) {
		return entity;
	}

	return nullptr;
}
