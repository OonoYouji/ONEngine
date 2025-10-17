#include "ImGuiInspectorWindow.h"

/// std
#include <format>

/// external
#include <imgui.h>

/// engine
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "../../../Math/ImGuiMath.h"
#include "Engine/Editor/EditorManager.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"


/// compute
#include "Engine/ECS/Component/Components/ComputeComponents/Light/Light.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Audio/AudioSource.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Effect/Effect.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
/// renderer
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line3DRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/ScreenPostEffectTag/ScreenPostEffectTag.h"


enum SelectedType {
	kNone,
	kEntity,
	kResource
};

ImGuiInspectorWindow::ImGuiInspectorWindow(const std::string& _windowName, EntityComponentSystem* _ecs, GraphicsResourceCollection* _resourceCollection, EditorManager* _editorManager)
	: pEcs_(_ecs), pGrc_(_resourceCollection), pEditorManager_(_editorManager) {
	windowName_ = _windowName;

	/// compute
	RegisterComponent<Transform>([&](IComponent* _comp) { COMP_DEBUG::TransformDebug(static_cast<Transform*>(_comp)); });
	RegisterComponent<DirectionalLight>([&](IComponent* _comp) { DirectionalLightDebug(static_cast<DirectionalLight*>(_comp)); });
	RegisterComponent<AudioSource>([&](IComponent* _comp) { COMP_DEBUG::AudioSourceDebug(static_cast<AudioSource*>(_comp)); });
	RegisterComponent<Variables>([&](IComponent* _comp) { COMP_DEBUG::VariablesDebug(static_cast<Variables*>(_comp)); });
	RegisterComponent<Effect>([&](IComponent* _comp) { COMP_DEBUG::EffectDebug(static_cast<Effect*>(_comp)); });
	RegisterComponent<Script>([&](IComponent* _comp) { COMP_DEBUG::ScriptDebug(static_cast<Script*>(_comp)); });
	RegisterComponent<Terrain>([&](IComponent* _comp) { COMP_DEBUG::TerrainDebug(static_cast<Terrain*>(_comp), pEcs_); });
	RegisterComponent<TerrainCollider>([&](IComponent* _comp) { COMP_DEBUG::TerrainColliderDebug(static_cast<TerrainCollider*>(_comp)); });
	RegisterComponent<GrassField>([&](IComponent* _comp) { COMP_DEBUG::GrassFieldDebug(static_cast<GrassField*>(_comp), pGrc_); });
	RegisterComponent<CameraComponent>([&](IComponent* _comp) { COMP_DEBUG::CameraDebug(static_cast<CameraComponent*>(_comp)); });

	/// renderer
	RegisterComponent<MeshRenderer>([&](IComponent* _comp) { COMP_DEBUG::MeshRendererDebug(static_cast<MeshRenderer*>(_comp)); });
	RegisterComponent<CustomMeshRenderer>([&](IComponent* _comp) { CustomMeshRendererDebug(static_cast<CustomMeshRenderer*>(_comp)); });
	RegisterComponent<SpriteRenderer>([&](IComponent* _comp) { COMP_DEBUG::SpriteDebug(static_cast<SpriteRenderer*>(_comp), pGrc_); });
	RegisterComponent<Line2DRenderer>([&]([[maybe_unused]] IComponent* _comp) {});
	RegisterComponent<Line3DRenderer>([&]([[maybe_unused]] IComponent* _comp) {});
	RegisterComponent<SkinMeshRenderer>([&](IComponent* _comp) { COMP_DEBUG::SkinMeshRendererDebug(static_cast<SkinMeshRenderer*>(_comp)); });
	RegisterComponent<ScreenPostEffectTag>([&](IComponent* _comp) { COMP_DEBUG::ScreenPostEffectTagDebug(static_cast<ScreenPostEffectTag*>(_comp)); });
	RegisterComponent<Skybox>([&](IComponent* _comp) { COMP_DEBUG::SkyboxDebug(static_cast<Skybox*>(_comp)); });

	/// collider
	RegisterComponent<SphereCollider>([&](IComponent* _comp) { COMP_DEBUG::SphereColliderDebug(static_cast<SphereCollider*>(_comp)); });
	RegisterComponent<BoxCollider>([&](IComponent* _comp) { COMP_DEBUG::BoxColliderDebug(static_cast<BoxCollider*>(_comp)); });


	/// 関数を登録
	inspectorFunctions_.emplace_back([]() {});
	inspectorFunctions_.emplace_back([this]() { EntityInspector(); });

}


void ImGuiInspectorWindow::ShowImGui() {
	if (!ImGui::Begin(windowName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	SelectedType selectedType = kNone;
	if (selectedEntity_) {
		selectedType = kEntity;
	}

	inspectorFunctions_[selectedType]();

	ImGui::End();
}

void ImGuiInspectorWindow::SetSelectedEntity(GameEntity* _entity) {
	selectedEntity_ = _entity;
}


void ImGuiInspectorWindow::EntityInspector() {
	uint64_t pointerValue = reinterpret_cast<uint64_t>(selectedEntity_->GetTransform());
	if (pointerValue == 0xdddddddddddddddd) {
		return;
	}

	/// ----------------------------
	/// 適当な編集の機能
	/// ----------------------------
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save")) {
				pEditorManager_->ExecuteCommand<EntityDataOutputCommand>(selectedEntity_);
			}

			if (ImGui::MenuItem("Load")) {
				pEditorManager_->ExecuteCommand<EntityDataInputCommand>(selectedEntity_);
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Apply Prefab")) {

			if (!selectedEntity_->GetPrefabName().empty()) {
				pEditorManager_->ExecuteCommand<CreatePrefabCommand>(selectedEntity_);
				pEcs_->ReloadPrefab(selectedEntity_->GetPrefabName());
			} else {
				Console::LogError("This entity is not a prefab instance.");
			}

		}

		ImGui::EndMenuBar();
	}

	if (!selectedEntity_->GetPrefabName().empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0, 0, 1));
		ImGuiInputTextReadOnly("entity prefab name", selectedEntity_->GetPrefabName());
		ImGui::PopStyleColor();
	}

	ImGuiInputTextReadOnly("entity name", selectedEntity_->GetName());
	ImGuiInputTextReadOnly("entity id", "Entity ID: " + std::to_string(selectedEntity_->GetId()));

	ImGui::Separator();
	/// ----------------------------
	/// componentのデバッグ
	/// ----------------------------
	for (auto itr = selectedEntity_->GetComponents().begin(); itr != selectedEntity_->GetComponents().end(); ) {
		std::pair<size_t, IComponent*> component = *itr;
		std::string componentName = typeid(*component.second).name();
		if (componentName.find("class ") == 0) {
			componentName = componentName.substr(6);
		}

		std::string label = componentName + "##" + std::to_string(reinterpret_cast<uintptr_t>(component.second));

		/// Idの追加(string)
		ImGui::PushID(label.c_str());

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


		/// ==============================================
		/// Componentのデバッグ表示ヘッダー
		/// ==============================================
		bool isHeaderOpen = ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		/// ==============================================
		/// ドラッグソースの開始
		/// ==============================================
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("Component", &component.second, sizeof(IComponent*));
			ImGui::Text("%s", componentName.c_str());
			ImGui::EndDragDropSource();
		}

		/// ==============================================
		/// 実際のComponentごとのデバッグ表示
		/// ==============================================
		if(isHeaderOpen) {
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
				auto resultItr = selectedEntity_->GetComponents().begin();
				pEditorManager_->ExecuteCommand<RemoveComponentCommand>(selectedEntity_, componentName, &resultItr);
				itr = resultItr; // イテレータを更新

				/// endじゃないかチェック
				if (itr == selectedEntity_->GetComponents().end()) {
					ImGui::EndPopup();
					break; // もしendに到達したらループを抜ける
				}

			}

			if (ImGui::MenuItem("reset")) {
				IComponent* comp = selectedEntity_->GetComponent(componentName);
				comp->Reset();
			}

			ImGui::EndPopup();
		}

		/// Idの削除
		ImGui::PopID();

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
				pEditorManager_->ExecuteCommand<AddComponentCommand>(selectedEntity_, name.second);
			}

			ImGui::PopStyleColor();
		}

		ImGui::EndPopup();
	}


}

GameEntity* ImGuiInspectorWindow::GetSelectedEntity() const {
	return selectedEntity_;
}
