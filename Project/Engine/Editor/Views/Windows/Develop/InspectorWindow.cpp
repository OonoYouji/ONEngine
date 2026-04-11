#include "InspectorWindow.h"


/// std
#include <format>

/// external
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Editor/Commands/ComponentEditCommands/ComponentEditCommands.h"
#include "Engine/Editor/Commands/WorldEditorCommands/WorldEditorCommands.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"

/// editor
#include "Engine/Editor/Manager/EditorManager.h"
#include "Engine/Editor/Math/ImGuiMath.h"
#include "Engine/Editor/Math/ImGuiSelection.h"

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
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
/// renderer
#include "Engine/ECS/Component/Components/RendererComponents/Skybox/Skybox.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/MeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/CustomMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Mesh/DissolveMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/SkinMesh/SkinMeshRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Sprite/SpriteRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line2DRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/Primitive/Line3DRenderer.h"
#include "Engine/ECS/Component/Components/RendererComponents/ScreenPostEffectTag/ScreenPostEffectTag.h"


using namespace ONEngine;

namespace Editor {


InspectorWindow::InspectorWindow(const std::string& windowName, DxManager* dxm, EntityComponentSystem* ecs, AssetCollection* assetCollection, EditorManager* editorManager)
	: pEcs_(ecs), pDxManager_(dxm), pAssetCollection_(assetCollection), pEditorManager_(editorManager) {
	windowName_ = windowName;

	/// ---------------------------------------------------
	/// 各ComponentのImGui関数登録
	/// ---------------------------------------------------

	/// compute
	RegisterComponent<Transform>([&](IComponent* comp) { ComponentDebug::TransformDebug(static_cast<Transform*>(comp)); });
	RegisterComponent<DirectionalLight>([&](IComponent* comp) { DirectionalLightDebug(static_cast<DirectionalLight*>(comp)); });
	RegisterComponent<AudioSource>([&](IComponent* comp) { ComponentDebug::AudioSourceDebug(static_cast<AudioSource*>(comp)); });
	RegisterComponent<Variables>([&](IComponent* comp) { ComponentDebug::VariablesDebug(static_cast<Variables*>(comp)); });
	RegisterComponent<Effect>([&](IComponent* comp) { ComponentDebug::EffectDebug(static_cast<Effect*>(comp)); });
	RegisterComponent<Script>([&](IComponent* comp) { ComponentDebug::ScriptDebug(static_cast<Script*>(comp)); });
	RegisterComponent<Terrain>([&](IComponent* comp) { ComponentDebug::TerrainDebug(static_cast<Terrain*>(comp), pEcs_, pAssetCollection_); });
	RegisterComponent<TerrainCollider>([&](IComponent* comp) { ComponentDebug::TerrainColliderDebug(static_cast<TerrainCollider*>(comp)); });
	RegisterComponent<GrassField>([&](IComponent* comp) { ComponentDebug::GrassFieldDebug(static_cast<GrassField*>(comp), pAssetCollection_); });
	RegisterComponent<CameraComponent>([&](IComponent* comp) { ComponentDebug::CameraDebug(static_cast<CameraComponent*>(comp)); });
	RegisterComponent<ShadowCaster>([&](IComponent* comp) { ComponentDebug::ShadowCasterDebug(static_cast<ShadowCaster*>(comp)); });
	RegisterComponent<VoxelTerrain>([&](IComponent* comp) { ComponentDebug::VoxelTerrainDebug(static_cast<VoxelTerrain*>(comp), pDxManager_, pAssetCollection_); });

	/// renderer
	RegisterComponent<MeshRenderer>([&](IComponent* comp) { ComponentDebug::MeshRendererDebug(static_cast<MeshRenderer*>(comp), pAssetCollection_); });
	RegisterComponent<CustomMeshRenderer>([&](IComponent* comp) { CustomMeshRendererDebug(static_cast<CustomMeshRenderer*>(comp)); });
	RegisterComponent<DissolveMeshRenderer>([&](IComponent* comp) { ShowGUI(static_cast<DissolveMeshRenderer*>(comp), pAssetCollection_); });
	RegisterComponent<SpriteRenderer>([&](IComponent* comp) { ComponentDebug::SpriteDebug(static_cast<SpriteRenderer*>(comp), pAssetCollection_); });
	RegisterComponent<Line2DRenderer>([&]([[maybe_unused]] IComponent* comp) {});
	RegisterComponent<Line3DRenderer>([&]([[maybe_unused]] IComponent* comp) {});
	RegisterComponent<SkinMeshRenderer>([&](IComponent* comp) { ComponentDebug::SkinMeshRendererDebug(static_cast<SkinMeshRenderer*>(comp)); });
	RegisterComponent<ScreenPostEffectTag>([&](IComponent* comp) { ComponentDebug::ScreenPostEffectTagDebug(static_cast<ScreenPostEffectTag*>(comp)); });
	RegisterComponent<Skybox>([&](IComponent* comp) { ComponentDebug::SkyboxDebug(static_cast<Skybox*>(comp)); });

	/// collider
	RegisterComponent<SphereCollider>([&](IComponent* comp) { ComponentDebug::SphereColliderDebug(static_cast<SphereCollider*>(comp)); });
	RegisterComponent<BoxCollider>([&](IComponent* comp) { ComponentDebug::BoxColliderDebug(static_cast<BoxCollider*>(comp)); });



	/// ---------------------------------------------------
	/// 関数を登録(SelectionTypeの順番に)
	/// ---------------------------------------------------

	inspectorFunctions_.resize(static_cast<size_t>(SelectionType::Count));
	inspectorFunctions_[static_cast<size_t>(SelectionType::None)] = ([this]() {});
	inspectorFunctions_[static_cast<size_t>(SelectionType::Entity)] = ([this]() { EntityInspector(); });
	inspectorFunctions_[static_cast<size_t>(SelectionType::Asset)] = ([this]() { AssetInspector(); });
	inspectorFunctions_[static_cast<size_t>(SelectionType::Script)] = ([this]() {});
}


void InspectorWindow::ShowImGui() {
	if(!ImGui::Begin(windowName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	SelectionType type = ImGuiSelection::GetSelectionType();
	inspectorFunctions_[static_cast<size_t>(type)]();

	ImGui::End();
}


void InspectorWindow::EntityInspector() {

	/// guidの取得、無効値なら抜ける
	const Guid& selectionGuid = ImGuiSelection::GetSelectedObject();
	if(!selectionGuid.CheckValid()) { return; }

	/// 選択しているエンティティの検索、見つからなければ即時終了
	GameEntity* selectedEntity = GetSelectedEntity(selectionGuid);
	if(!selectedEntity) { return; }



	ShowEntityMenuBar(selectedEntity);
	ShowEntityBasicInfo(selectedEntity);

	ImGui::Separator();

	ShowEntityComponents(selectedEntity);

	/// ----------------------------
	/// componentの追加
	/// ----------------------------

	ShowAddComponentPopup(selectedEntity);

}

///
/// 選択しているエンティティを検索、選択していなければnullptrを返す
///
GameEntity* InspectorWindow::GetSelectedEntity(const ONEngine::Guid& entityGuid) {
	/// 選択しているオブジェクトがGroup違いの場合もあるのですべてのGroupを探索する。
	/// Guidの被りはない想定なので見つかったら即返す。

	GameEntity* res = nullptr;
	for(auto& group : pEcs_->GetECSGroups()) {
		res = group.second->GetEntityFromGuid(entityGuid);
		if(res) { return res; }
	}

	return nullptr;
}

///
/// 選択しているエンティティのメニューバー表示を行う 
///
void InspectorWindow::ShowEntityMenuBar(ONEngine::GameEntity* entity) {
	if(ImGui::BeginMenuBar()) {

		/// エンティティの保存、読み込み
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("Save")) {
				pEditorManager_->ExecuteCommand<EntityDataOutputCommand>(entity);
			}

			if(ImGui::MenuItem("Load")) {
				pEditorManager_->ExecuteCommand<EntityDataInputCommand>(entity);
			}

			ImGui::EndMenu();
		}

		/// プレハブへの適用、プレハブがあれば
		if(ImGui::MenuItem("Apply Prefab")) {

			if(!entity->GetPrefabName().empty()) {
				pEditorManager_->ExecuteCommand<CreatePrefabCommand>(entity);
				pEcs_->ReloadPrefab(entity->GetPrefabName());
			} else {
				Console::LogError("This entity is not a prefab instance.");
			}

		}

		ImGui::EndMenuBar();
	}
}

/// 
/// エンティティの基本情報を表示する
/// 
void InspectorWindow::ShowEntityBasicInfo(ONEngine::GameEntity* entity) {

	/// プレハブがあるならプレハブ名を表示
	if(!entity->GetPrefabName().empty()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75f, 0, 0, 1));
		ImGuiInputTextReadOnly("entity prefab name", entity->GetPrefabName());
		ImGui::PopStyleColor();
	}

	/// その他エンティティの基本情報
	ImGuiInputTextReadOnly("entity name", entity->GetName());
	ImGuiInputTextReadOnly("entity id", "Entity ID: " + std::to_string(entity->GetId()));
	ImMathf::Checkbox("entity active", &entity->active);
}

///
/// エンティティのコンポーネントを表示する
///  
void InspectorWindow::ShowEntityComponents(ONEngine::GameEntity* entity) {

	/// for文の中で毎回生成するのは良くないので事前に用意
	std::string label = "", compName = "";

	for(auto itr = entity->GetComponents().begin(); itr != entity->GetComponents().end(); ) {
		std::pair<size_t, IComponent*> component = *itr;
		compName = GetComponentTypeName(component.second);
		label = compName + "##" + std::to_string(reinterpret_cast<uintptr_t>(component.second));

		/// Idの追加(string)
		ImGui::PushID(label.c_str());

		/// チェックボックスでenable/disableを切り替え
		bool enabled = component.second->enable;
		if(ImGui::Checkbox(("##" + label).c_str(), &enabled)) {
			component.second->enable = enabled;
		}

		ImGui::SameLine();


		/// アクティブ/非アクティブで表示を変える
		if(!enabled) {
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
		if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("Component", &component.second, sizeof(IComponent*));
			ImGui::Text("%s", compName.c_str());
			ImGui::EndDragDropSource();
		}

		/// ==============================================
		/// 実際のComponentごとのデバッグ表示
		/// ==============================================
		if(isHeaderOpen) {
			/// 右クリックでポップアップメニューを開く
			if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup(label.c_str());
			}

			ImGui::Indent(34.0f);
			if(componentDebugFuncs_.contains(component.first)) {
				componentDebugFuncs_[component.first](component.second);
			}
			ImGui::Unindent(34.0f);
		}



		if(!enabled) {
			ImGui::PopStyleColor();
		}


		if(ImGui::BeginPopupContextItem(label.c_str())) {
			if(ImGui::MenuItem("delete")) {
				auto resultItr = entity->GetComponents().begin();
				pEditorManager_->ExecuteCommand<RemoveComponentCommand>(entity, compName, &resultItr);
				itr = resultItr; // イテレータを更新

				/// endじゃないかチェック
				if(itr == entity->GetComponents().end()) {
					ImGui::EndPopup();
					ImGui::PopID();
					break; // もしendに到達したらループを抜ける
				}

			}

			if(ImGui::MenuItem("reset")) {
				IComponent* comp = entity->GetComponent(compName);
				comp->Reset();
			}

			ImGui::EndPopup();
		}

		/// Idの削除
		ImGui::PopID();

		++itr;
	}
}

///
/// エンティティに対してコンポーネントを追加するためのポップアップ
///
void InspectorWindow::ShowAddComponentPopup(ONEngine::GameEntity* entity) {
	ImGui::Separator();

	const float indentSize = 4 * ImGui::GetStyle().IndentSpacing;
	ImGui::Indent(indentSize);

	const ImVec2 openPopupButtonSize = ImVec2(256.0f, 32.0f);
	if(ImGui::Button("Add Component", openPopupButtonSize)) {
		ImGui::OpenPopup("AddComponent");
	}

	ImGui::Unindent(indentSize);



	if(ImGui::BeginPopup("AddComponent", ImGuiWindowFlags_AlwaysVerticalScrollbar)) {

		ImVec2 buttonSize = ImVec2(128.0f, 24.0f);
		for(const auto& name : componentNames_) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if(ImGui::Button(name.second.c_str(), buttonSize)) {
				pEditorManager_->ExecuteCommand<AddComponentCommand>(entity, name.second);
			}

			ImGui::PopStyleColor();
		}

		ImGui::EndPopup();
	}
}

void InspectorWindow::AssetInspector() {
	/// Typeごとに表示を変える

	AssetType type = pAssetCollection_->GetAssetTypeFromGuid(ImGuiSelection::GetSelectedObject());

	switch(type) {
	case AssetType::Texture:
	{
		ImGui::Text("Texture Inspector");
		Texture* texture = pAssetCollection_->GetTextureFromGuid(ImGuiSelection::GetSelectedObject());
		if(texture) {
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

void InspectorWindow::TextureAssetInspector(Texture* tex) {
	/// ----- テクスチャのインスペクター表示 ----- /

	/// previewのための枠を確保
	ImGui::Text("Texture Preview:");
	ImVec2 availSize = ImGui::GetContentRegionAvail();
	const Vector2& textureSize = tex->GetTextureSize();
	ImVec2 displaySize = ImMathf::CalculateAspectFitSize(textureSize, availSize);

	/// Guidの表示
	ImGuiInputTextReadOnly("Texture Guid", tex->guid.ToString());

	/// 枠を表示
	ImGui::BeginChild("TextureFrame", displaySize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Image((ImTextureID)(uintptr_t)tex->GetSRVGPUHandle().ptr, displaySize);
	ImGui::EndChild();
}

} /// namespace Editor