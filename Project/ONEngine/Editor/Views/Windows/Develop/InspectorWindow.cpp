#include "InspectorWindow.h"

/// std
#include <format>

/// external
#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

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
#include "Engine/Editor/Math/MetaData/AssetMetaReflection.h"

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


InspectorWindow::InspectorWindow(const std::string& windowName, DxManager* dxm, EntityComponentSystem* ecs, Asset::AssetCollection* assetCollection, EditorManager* editorManager)
	: pEcs_(ecs), pDxManager_(dxm), pAssetCollection_(assetCollection), pEditorManager_(editorManager) {
	windowName_ = windowName;

	/// ---------------------------------------------------
	/// 各ComponentのImGui関数登録
	/// ---------------------------------------------------

	/// compute
	RegisterComponent<Transform>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::TransformDebug(static_cast<Transform*>(comp)); });
	RegisterComponent<DirectionalLight>(ComponentType::Compute, [&](IComponent* comp) { DirectionalLightDebug(static_cast<DirectionalLight*>(comp)); });
	RegisterComponent<AudioSource>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::AudioSourceDebug(static_cast<AudioSource*>(comp)); });
	RegisterComponent<Variables>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::VariablesDebug(static_cast<Variables*>(comp)); });
	RegisterComponent<Effect>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::EffectDebug(static_cast<Effect*>(comp)); });
	RegisterComponent<Terrain>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::TerrainDebug(static_cast<Terrain*>(comp), pEcs_, pAssetCollection_); });
	RegisterComponent<TerrainCollider>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::TerrainColliderDebug(static_cast<TerrainCollider*>(comp)); });
	RegisterComponent<GrassField>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::GrassFieldDebug(static_cast<GrassField*>(comp), pAssetCollection_); });
	RegisterComponent<CameraComponent>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::CameraDebug(static_cast<CameraComponent*>(comp)); });
	RegisterComponent<ShadowCaster>(ComponentType::Compute, [&](IComponent* comp) { ComponentDebug::ShadowCasterDebug(static_cast<ShadowCaster*>(comp)); });

	RegisterComponent<Script>(ComponentType::Script, [&](IComponent* comp) { ComponentDebug::ScriptDebug(static_cast<Script*>(comp)); });

	/// renderer
	RegisterComponent<VoxelTerrain>(ComponentType::Renderer, [&](IComponent* comp) { ComponentDebug::VoxelTerrainDebug(static_cast<VoxelTerrain*>(comp), pDxManager_, pAssetCollection_); });
	RegisterComponent<MeshRenderer>(ComponentType::Renderer, [&](IComponent* comp) { ComponentDebug::MeshRendererDebug(static_cast<MeshRenderer*>(comp), pAssetCollection_); });
	RegisterComponent<CustomMeshRenderer>(ComponentType::Renderer, [&](IComponent* comp) { CustomMeshRendererDebug(static_cast<CustomMeshRenderer*>(comp)); });
	RegisterComponent<DissolveMeshRenderer>(ComponentType::Renderer, [&](IComponent* comp) { ShowGUI(static_cast<DissolveMeshRenderer*>(comp), pAssetCollection_); });
	RegisterComponent<SpriteRenderer>(ComponentType::Renderer, [&](IComponent* comp) { ComponentDebug::SpriteDebug(static_cast<SpriteRenderer*>(comp), pAssetCollection_); });
	RegisterComponent<Line2DRenderer>(ComponentType::Renderer, [&]([[maybe_unused]] IComponent* comp) {});
	RegisterComponent<Line3DRenderer>(ComponentType::Renderer, [&]([[maybe_unused]] IComponent* comp) {});
	RegisterComponent<SkinMeshRenderer>(ComponentType::Renderer, [&](IComponent* comp) { ComponentDebug::SkinMeshRendererDebug(static_cast<SkinMeshRenderer*>(comp), pAssetCollection_); });
	RegisterComponent<ScreenPostEffectTag>(ComponentType::Renderer, [&](IComponent* comp) { ComponentDebug::ScreenPostEffectTagDebug(static_cast<ScreenPostEffectTag*>(comp)); });
	RegisterComponent<Skybox>(ComponentType::Renderer, [&](IComponent* comp) { ComponentDebug::SkyboxDebug(static_cast<Skybox*>(comp)); });

	/// collider
	RegisterComponent<SphereCollider>(ComponentType::Collider, [&](IComponent* comp) { ComponentDebug::SphereColliderDebug(static_cast<SphereCollider*>(comp)); });
	RegisterComponent<BoxCollider>(ComponentType::Collider, [&](IComponent* comp) { ComponentDebug::BoxColliderDebug(static_cast<BoxCollider*>(comp)); });



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
	for(auto itr = entity->GetComponents().begin(); itr != entity->GetComponents().end(); ) {
		DrawComponentNode(entity, itr);
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

	if(ImGui::BeginPopup("AddComponent")) {

		/// ==============================================
		/// 1. 検索バーの描画と入力処理
		/// ==============================================
		static char searchBuffer[256] = ""; // 検索用バッファ

		// ポップアップが開かれた瞬間にバッファをクリアし、フォーカスを当てる
		if(ImGui::IsWindowAppearing()) {
			memset(searchBuffer, 0, sizeof(searchBuffer));
			ImGui::SetKeyboardFocusHere();
		}

		// Hint付きの入力フィールド（何も入力されていない時に薄い文字を出す）
		ImGui::InputTextWithHint("##SearchComp", "Search Component...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
		ImGui::Separator();

		// 入力された検索文字列を小文字に変換（大文字小文字を区別しない検索のため）
		std::string searchStr = searchBuffer;
		std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), [](unsigned char c) { return std::tolower(c); });
		bool isSearching = !searchStr.empty();

		/// ==============================================
		/// 2. コンポーネントをTypeごとに分類する
		/// ==============================================
		std::map<ComponentType, std::vector<std::string>> categorizedComponents;
		for(const auto& uiBinding : componentUIBindings_) {
			categorizedComponents[uiBinding.second.type].push_back(uiBinding.second.name);
		}

		/// ==============================================
		/// 3. メニューまたは検索結果の描画
		/// ==============================================
		for(auto& [type, names] : categorizedComponents) {
			std::sort(names.begin(), names.end());

			if(isSearching) {
				// ----------------------------------------
				// 検索中：カテゴリを無視してマッチしたものをフラット表示
				// ----------------------------------------
				for(const auto& name : names) {
					// コンポーネント名も小文字に変換して比較
					std::string lowerName = name;
					std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return std::tolower(c); });

					// 検索文字列が含まれていたら表示
					if(lowerName.find(searchStr) != std::string::npos) {
						if(ImGui::MenuItem(name.c_str())) {
							pEditorManager_->ExecuteCommand<AddComponentCommand>(entity, name);
							ImGui::CloseCurrentPopup(); // 追加したらポップアップを閉じる
						}
					}
				}
			} else {
				// ----------------------------------------
				// 検索していない時：カテゴリごとにサブメニュー化
				// ----------------------------------------
				std::string typeName{ magic_enum::enum_name(type) };
				if(ImGui::BeginMenu(typeName.c_str())) {
					for(const auto& name : names) {
						if(ImGui::MenuItem(name.c_str())) {
							pEditorManager_->ExecuteCommand<AddComponentCommand>(entity, name);
							ImGui::CloseCurrentPopup(); // 追加したらポップアップを閉じる
						}
					}
					ImGui::EndMenu();
				}
			}
		}

		ImGui::EndPopup();
	}
}


///
/// コンポーネントタイプごとに色を取得する
///
ImVec4 InspectorWindow::GetComponentBaseColor(ComponentType type) const {
	switch(type) {
	case ComponentType::Compute:  return ImVec4(0.15f, 0.30f, 0.45f, 0.70f);
	case ComponentType::Renderer: return ImVec4(0.20f, 0.40f, 0.25f, 0.70f);
	case ComponentType::Collider: return ImVec4(0.50f, 0.30f, 0.15f, 0.70f);
	default:                      return ImGui::GetStyleColorVec4(ImGuiCol_Header);
	}
	return ImVec4();
}


///
/// コンポーネントのエディタ表示 
///
void InspectorWindow::DrawComponentNode(ONEngine::GameEntity* entity, auto& itr) {
	IComponent* comp = itr->second;
	std::string compName = GetComponentTypeName(comp);
	std::string label = compName + "##" + std::to_string(reinterpret_cast<uintptr_t>(comp));

	ImGui::PushID(label.c_str());

	// 色の決定
	ComponentType compType = componentUIBindings_.contains(itr->first) ? componentUIBindings_[itr->first].type : ComponentType::Compute;
	ImVec4 baseColor = GetComponentBaseColor(compType);

	// 1. ヘッダーの描画（開いているかどうかを取得）
	bool isHeaderOpen = DrawComponentHeaderUI(comp, compName, baseColor);

	// ドラッグ＆ドロップソースの処理
	if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		ImGui::SetDragDropPayload("Component", &comp, sizeof(IComponent*));
		ImGui::Text("%s", compName.c_str());
		ImGui::EndDragDropSource();
	}

	bool isDeleted = false;

	// ヘッダーが開かれている場合の中身の処理
	if(isHeaderOpen) {
		// 2. ポップアップメニューの処理 (削除されたかどうかのフラグを受け取る)
		isDeleted = HandleComponentPopupMenu(entity, comp, compName, itr);

		// 3. 削除されていなければ中身のプロパティを描画
		if(!isDeleted) {
			DrawComponentInnerContent(comp, itr->first, comp->enable);
		}

		// TreeNodeExを開いた場合は必ずTreePopを呼ぶ
		ImGui::TreePop();
	}

	ImGui::PopID();

	// 削除されていなければイテレータを次に進める
	if(!isDeleted) {
		++itr;
	}
}

bool InspectorWindow::DrawComponentHeaderUI(ONEngine::IComponent* comp, const std::string& compName, ImVec4 baseColor) {
	ImGui::PushStyleColor(ImGuiCol_Header, baseColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(baseColor.x + 0.05f, baseColor.y + 0.05f, baseColor.z + 0.05f, 0.8f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

	// TreeNode
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
	bool isHeaderOpen = ImGui::TreeNodeEx("##header", flags, "");

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2.0f);

	// チェックボックス
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1, 1, 1, 0.1f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1, 1, 1, 0.2f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

	bool enabled = comp->enable;
	if(ImGui::Checkbox("##enabled", &enabled)) {
		comp->enable = enabled;
	}
	ImGui::PopStyleColor(4);

	// アイコンと名前
	ImGui::SameLine();
	if(!enabled) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
	ImGui::TextDisabled("(?)");
	ImGui::SameLine();
	ImGui::TextUnformatted(compName.c_str());
	if(!enabled) ImGui::PopStyleColor();

	// 設定ボタン (ギア)
	float button_size = ImGui::GetFrameHeight();
	ImGui::SameLine(ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - button_size - 4.0f);
	if(ImGui::Button("::", ImVec2(button_size, button_size))) {
		ImGui::OpenPopup("CompPopup");
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(2);

	return isHeaderOpen;
}

bool InspectorWindow::HandleComponentPopupMenu(ONEngine::GameEntity* entity, ONEngine::IComponent* comp, const std::string& compName, auto& itr) {
	bool isDeleted = false;

	if(ImGui::BeginPopup("CompPopup")) {
		if(ImGui::MenuItem("Reset")) { comp->Reset(); }
		ImGui::Separator();

		if(ImGui::MenuItem("Remove Component")) {
			auto resultItr = entity->GetComponents().begin();
			pEditorManager_->ExecuteCommand<RemoveComponentCommand>(entity, compName, &resultItr);
			itr = resultItr;
			isDeleted = true;
		}
		ImGui::EndPopup();
	}

	return isDeleted;
}

void InspectorWindow::DrawComponentInnerContent(ONEngine::IComponent* comp, size_t componentTypeId, bool enabled) {
	ImGui::Indent(22.0f);
	if(!enabled) ImGui::BeginDisabled();

	if(componentUIBindings_.contains(componentTypeId)) {
		componentUIBindings_[componentTypeId].function(comp);
	}

	if(!enabled) ImGui::EndDisabled();
	ImGui::Unindent(22.0f);
}

void InspectorWindow::AssetInspector() {
	/// Typeごとに表示を変える

	Asset::AssetType type = pAssetCollection_->GetAssetTypeFromGuid(ImGuiSelection::GetSelectedObject());

	switch(type) {
	case Asset::AssetType::Texture:
	{
		ImGui::Text("Texture Inspector");
		ONEngine::Asset::Texture* texture = pAssetCollection_->GetTextureFromGuid(ImGuiSelection::GetSelectedObject());
		if(texture) {
			TextureAssetInspector(texture);
		}

	}
	break;
	case Asset::AssetType::Audio:
		ImGui::Text("Audio Inspector");
		break;
	case Asset::AssetType::Mesh:
		ImGui::Text("Mesh Inspector");
		break;
	case Asset::AssetType::Material:
		ImGui::Text("Material Inspector");

		/*
		* use shader     : 使用するshader
		* albedo color	 : ベースの色
		* albedo texture : 使用するテクスチャ
		* used normal	 : 法線マップを使用するかどうか
		* normal texture : 使用する法線マップ
		*/




		break;
	}


	{
		static ONEngine::Asset::Texture::MetaData meta{};
		DrawMetaUI(meta);
	}

	{
		static ONEngine::Asset::AudioClip::MetaData meta{};
		DrawMetaUI(meta);
	}

	{
		static ONEngine::Asset::Material::MetaData meta{};
		DrawMetaUI(meta);
	}

	{
		static ONEngine::Asset::Shader::MetaData meta{};
		DrawMetaUI(meta);
	}

	{
		static ONEngine::Asset::Model::MetaData meta{};
		DrawMetaUI(meta);
	}



}

void InspectorWindow::TextureAssetInspector(ONEngine::Asset::Texture* tex) {
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