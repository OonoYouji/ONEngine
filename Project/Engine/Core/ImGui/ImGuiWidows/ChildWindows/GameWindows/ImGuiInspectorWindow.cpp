#include "ImGuiInspectorWindow.h"

/// std
#include <format>

/// external
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "../../../Math/ImGuiMath.h"
#include "Engine/Core/ImGui/ImGuiSelection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
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


//enum SelectedType {
//	kNone,
//	kEntity,
//	kResource
//};

ImGuiInspectorWindow::ImGuiInspectorWindow(const std::string& _windowName, EntityComponentSystem* _ecs, AssetCollection* _assetCollection, EditorManager* _editorManager)
	: pEcs_(_ecs), pAssetCollection_(_assetCollection), pEditorManager_(_editorManager) {
	windowName_ = _windowName;

	/// compute
	RegisterComponent<Transform>([&](IComponent* _comp) { COMP_DEBUG::TransformDebug(static_cast<Transform*>(_comp)); });
	RegisterComponent<DirectionalLight>([&](IComponent* _comp) { DirectionalLightDebug(static_cast<DirectionalLight*>(_comp)); });
	RegisterComponent<AudioSource>([&](IComponent* _comp) { COMP_DEBUG::AudioSourceDebug(static_cast<AudioSource*>(_comp)); });
	RegisterComponent<Variables>([&](IComponent* _comp) { COMP_DEBUG::VariablesDebug(static_cast<Variables*>(_comp)); });
	RegisterComponent<Effect>([&](IComponent* _comp) { COMP_DEBUG::EffectDebug(static_cast<Effect*>(_comp)); });
	RegisterComponent<Script>([&](IComponent* _comp) { COMP_DEBUG::ScriptDebug(static_cast<Script*>(_comp)); });
	RegisterComponent<Terrain>([&](IComponent* _comp) { COMP_DEBUG::TerrainDebug(static_cast<Terrain*>(_comp), pEcs_, pAssetCollection_); });
	RegisterComponent<TerrainCollider>([&](IComponent* _comp) { COMP_DEBUG::TerrainColliderDebug(static_cast<TerrainCollider*>(_comp)); });
	RegisterComponent<GrassField>([&](IComponent* _comp) { COMP_DEBUG::GrassFieldDebug(static_cast<GrassField*>(_comp), pAssetCollection_); });
	RegisterComponent<CameraComponent>([&](IComponent* _comp) { COMP_DEBUG::CameraDebug(static_cast<CameraComponent*>(_comp)); });

	/// renderer
	RegisterComponent<MeshRenderer>([&](IComponent* _comp) { COMP_DEBUG::MeshRendererDebug(static_cast<MeshRenderer*>(_comp), pAssetCollection_); });
	RegisterComponent<CustomMeshRenderer>([&](IComponent* _comp) { CustomMeshRendererDebug(static_cast<CustomMeshRenderer*>(_comp)); });
	RegisterComponent<SpriteRenderer>([&](IComponent* _comp) { COMP_DEBUG::SpriteDebug(static_cast<SpriteRenderer*>(_comp), pAssetCollection_); });
	RegisterComponent<Line2DRenderer>([&]([[maybe_unused]] IComponent* _comp) {});
	RegisterComponent<Line3DRenderer>([&]([[maybe_unused]] IComponent* _comp) {});
	RegisterComponent<SkinMeshRenderer>([&](IComponent* _comp) { COMP_DEBUG::SkinMeshRendererDebug(static_cast<SkinMeshRenderer*>(_comp)); });
	RegisterComponent<ScreenPostEffectTag>([&](IComponent* _comp) { COMP_DEBUG::ScreenPostEffectTagDebug(static_cast<ScreenPostEffectTag*>(_comp)); });
	RegisterComponent<Skybox>([&](IComponent* _comp) { COMP_DEBUG::SkyboxDebug(static_cast<Skybox*>(_comp)); });

	/// collider
	RegisterComponent<SphereCollider>([&](IComponent* _comp) { COMP_DEBUG::SphereColliderDebug(static_cast<SphereCollider*>(_comp)); });
	RegisterComponent<BoxCollider>([&](IComponent* _comp) { COMP_DEBUG::BoxColliderDebug(static_cast<BoxCollider*>(_comp)); });


	/// 関数を登録(SelectionTypeの順番に)
	/// SelectionType::None
	inspectorFunctions_.emplace_back([]() {});

	/// SelectionType::Entity
	inspectorFunctions_.emplace_back([this]() { EntityInspector(); });

	/// SelectionType::Asset
	inspectorFunctions_.emplace_back([this]() {  AssetInspector();  });

	/// SelectionType::Script
	inspectorFunctions_.emplace_back([]() {});

}


void ImGuiInspectorWindow::ShowImGui() {
	if (!ImGui::Begin(windowName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	SelectionType type = ImGuiSelection::GetSelectionType();
	inspectorFunctions_[static_cast<size_t>(type)]();

	ImGui::End();
}


void ImGuiInspectorWindow::EntityInspector() {

	/// guidの取得、無効値なら抜ける
	const Guid& selectionGuid = ImGuiSelection::GetSelectedObject();
	if (!selectionGuid.CheckValid()) {
		return;
	}

	/// アセットなら抜ける
	bool isAsset = pAssetCollection_->IsAssetExist(selectionGuid);
	if (isAsset) {
		return;
	}

	GameEntity* selectedEntity = nullptr;
	for(auto& group : pEcs_->GetECSGroups()) {
		selectedEntity = group.second->GetEntityFromGuid(selectionGuid);
		if (selectedEntity) {
			break;
		}
	}

	if (!selectedEntity) {
		return;
	}

	/// ----------------------------
	/// 適当な編集の機能
	/// ----------------------------
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save")) {
				pEditorManager_->ExecuteCommand<EntityDataOutputCommand>(selectedEntity);
			}

			if (ImGui::MenuItem("Load")) {
				pEditorManager_->ExecuteCommand<EntityDataInputCommand>(selectedEntity);
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Apply Prefab")) {

			if (!selectedEntity->GetPrefabName().empty()) {
				pEditorManager_->ExecuteCommand<CreatePrefabCommand>(selectedEntity);
				pEcs_->ReloadPrefab(selectedEntity->GetPrefabName());
			} else {
				Console::LogError("This entity is not a prefab instance.");
			}

		}

		ImGui::EndMenuBar();
	}

	if (!selectedEntity->GetPrefabName().empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0, 0, 1));
		ImGuiInputTextReadOnly("entity prefab name", selectedEntity->GetPrefabName());
		ImGui::PopStyleColor();
	}

	ImGuiInputTextReadOnly("entity name", selectedEntity->GetName());
	ImGuiInputTextReadOnly("entity id", "Entity ID: " + std::to_string(selectedEntity->GetId()));

	ImGui::Separator();
	/// ----------------------------
	/// componentのデバッグ
	/// ----------------------------
	for (auto itr = selectedEntity->GetComponents().begin(); itr != selectedEntity->GetComponents().end(); ) {
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
		if (isHeaderOpen) {
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
				auto resultItr = selectedEntity->GetComponents().begin();
				pEditorManager_->ExecuteCommand<RemoveComponentCommand>(selectedEntity, componentName, &resultItr);
				itr = resultItr; // イテレータを更新

				/// endじゃないかチェック
				if (itr == selectedEntity->GetComponents().end()) {
					ImGui::EndPopup();
					break; // もしendに到達したらループを抜ける
				}

			}

			if (ImGui::MenuItem("reset")) {
				IComponent* comp = selectedEntity->GetComponent(componentName);
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
				pEditorManager_->ExecuteCommand<AddComponentCommand>(selectedEntity, name.second);
			}

			ImGui::PopStyleColor();
		}

		ImGui::EndPopup();
	}


}

void ImGuiInspectorWindow::AssetInspector() {
	/// Typeごとに表示を変える

	AssetType type = pAssetCollection_->GetAssetTypeFromGuid(ImGuiSelection::GetSelectedObject());

	switch (type) {
	case AssetType::Texture:
	{
		ImGui::Text("Texture Inspector");
		Texture* texture = pAssetCollection_->GetTextureFromGuid(ImGuiSelection::GetSelectedObject());
		if (texture) {
			TextureAssetInspector(texture);
		}

	}
	break;
	case AssetType::Audio:
		ImGui::Text("Audio Inspector");
		break;
	case AssetType::Mesh:
		ImGui::Text("Mesh Inspector");
		break;
	case AssetType::Material:
		ImGui::Text("Material Inspector");
		break;
	}

}

void ImGuiInspectorWindow::TextureAssetInspector(Texture* _texture) {
	/// ----- テクスチャのインスペクター表示 ----- /

	/// previewのための枠を確保
	ImGui::Text("Texture Preview:");
	ImVec2 availSize = ImGui::GetContentRegionAvail();
	const Vector2& textureSize = _texture->GetTextureSize();
	ImVec2 displaySize = ImMathf::CalculateAspectFitSize(textureSize, availSize);

	/// 枠を表示
	ImGui::BeginChild("TextureFrame", displaySize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Image((ImTextureID)(uintptr_t)_texture->GetSRVGPUHandle().ptr, displaySize);
	ImGui::EndChild();
}

