#include "InspectorWindow.h"

/// std
#include <format>
#include <algorithm>

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
#include "Engine/ECS/Component/Components/ComputeComponents/ParticleSystem/ParticleSystem.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Terrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/TerrainCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Terrain/Grass/GrassField.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Camera/CameraComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/BoxCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Collision/SphereCollider.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Component/Components/ComputeComponents/ShadowCaster/ShadowCaster.h"
#include "Engine/ECS/Component/Components/ComputeComponents/VoxelTerrain/VoxelTerrain.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Agent/AgentIntentComponent.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animator/Animator.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Animation/AnimationPlayer.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Transform/Transform.h"

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
	RegisterComponentMulti<Transform>(ComponentType::Compute, [&](const std::vector<Transform*>& comps) { ComponentDebug::TransformDebug(comps); });
	RegisterComponent<DirectionalLight>(ComponentType::Compute, [&](DirectionalLight* comp) { DirectionalLightDebug(comp); });
	RegisterComponent<AudioSource>(ComponentType::Compute, [&](AudioSource* comp) { ComponentDebug::AudioSourceDebug(comp); });
	RegisterComponent<Variables>(ComponentType::Compute, [&](Variables* comp) { ComponentDebug::VariablesDebug(comp); });
	RegisterComponent<AnimationPlayer>(ComponentType::Compute, [&](AnimationPlayer* comp) { ComponentDebug::AnimationPlayerDebug(comp); });
	RegisterComponent<Effect>(ComponentType::Compute, [&](Effect* comp) { ComponentDebug::EffectDebug(comp); });
	RegisterComponent<ParticleSystem>(ComponentType::Compute, [&](ParticleSystem* comp) { ONEngine::ParticleSystemDebug(comp); });
	RegisterComponent<Terrain>(ComponentType::Compute, [&](Terrain* comp) { ComponentDebug::TerrainDebug(comp, pEcs_, pAssetCollection_); });
	RegisterComponent<TerrainCollider>(ComponentType::Compute, [&](TerrainCollider* comp) { ComponentDebug::TerrainColliderDebug(comp); });
	RegisterComponent<GrassField>(ComponentType::Compute, [&](GrassField* comp) { ComponentDebug::GrassFieldDebug(comp, pAssetCollection_); });
	RegisterComponent<CameraComponent>(ComponentType::Compute, [&](CameraComponent* comp) { ComponentDebug::CameraDebug(comp); });
	RegisterComponent<ShadowCaster>(ComponentType::Compute, [&](ShadowCaster* comp) { ComponentDebug::ShadowCasterDebug(comp); });
	RegisterComponent<AgentIntentComponent>(ComponentType::Compute, [&](AgentIntentComponent* comp) { ComponentDebug::AgentIntentComponentDebug(comp); });
	RegisterComponentMulti<Animator>(ComponentType::Compute, [&](const std::vector<Animator*>& comps) { ComponentDebug::AnimatorDebug(comps); });

	RegisterComponent<Script>(ComponentType::Script, [&](Script* comp) { ComponentDebug::ScriptDebug(comp); });

	/// renderer
	RegisterComponent<VoxelTerrain>(ComponentType::Renderer, [&](VoxelTerrain* comp) { ComponentDebug::VoxelTerrainDebug(comp, pDxManager_, pAssetCollection_); });
	RegisterComponent<MeshRenderer>(ComponentType::Renderer, [&](MeshRenderer* comp) { ComponentDebug::MeshRendererDebug(comp, pAssetCollection_); });
	RegisterComponent<CustomMeshRenderer>(ComponentType::Renderer, [&](CustomMeshRenderer* comp) { CustomMeshRendererDebug(comp); });
	RegisterComponent<DissolveMeshRenderer>(ComponentType::Renderer, [&](DissolveMeshRenderer* comp) { ShowGUI(comp, pAssetCollection_); });
	RegisterComponent<SpriteRenderer>(ComponentType::Renderer, [&](SpriteRenderer* comp) { ComponentDebug::SpriteDebug(comp, pAssetCollection_); });
	RegisterComponent<Line2DRenderer>(ComponentType::Renderer, [&](Line2DRenderer* /*comp*/) {});
	RegisterComponent<Line3DRenderer>(ComponentType::Renderer, [&](Line3DRenderer* /*comp*/) {});
	RegisterComponent<SkinMeshRenderer>(ComponentType::Renderer, [&](SkinMeshRenderer* comp) { ComponentDebug::SkinMeshRendererDebug(comp, pAssetCollection_); });
	RegisterComponent<ScreenPostEffectTag>(ComponentType::Renderer, [&](ScreenPostEffectTag* comp) { ComponentDebug::ScreenPostEffectTagDebug(comp); });
	RegisterComponent<Skybox>(ComponentType::Renderer, [&](Skybox* comp) { ComponentDebug::SkyboxDebug(comp); });

	/// collider
	RegisterComponent<SphereCollider>(ComponentType::Collider, [&](SphereCollider* comp) { ComponentDebug::SphereColliderDebug(comp); });
	RegisterComponent<BoxCollider>(ComponentType::Collider, [&](BoxCollider* comp) { ComponentDebug::BoxColliderDebug(comp); });



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
	if (type == SelectionType::Entity) {
		EntityInspector();
	} else {
		inspectorFunctions_[static_cast<size_t>(type)]();
	}

	ImGui::End();
}


void InspectorWindow::EntityInspector() {

	/// 選択している全エンティティの取得
	std::vector<GameEntity*> selectedEntities = GetSelectedEntities();
	if(selectedEntities.empty()) { return; }

	if (selectedEntities.size() > 1) {
		MultiEntityInspector(selectedEntities);
		return;
	}

	GameEntity* selectedEntity = selectedEntities[0];

	ShowEntityMenuBar(selectedEntity);
	ShowEntityBasicInfo(selectedEntity);
	ImGui::Separator();
	ShowEntityComponents(selectedEntity);
	ShowAddComponentPopup(selectedEntity);

}

void InspectorWindow::MultiEntityInspector(const std::vector<ONEngine::GameEntity*>& entities) {
	ImGui::Text("%zu objects selected", entities.size());
	ImGui::Separator();

	ShowMultiEntityBasicInfo(entities);
	ImGui::Separator();
	ShowMultiEntityComponents(entities);
	ShowMultiAddComponentPopup(entities);
}

std::vector<GameEntity*> InspectorWindow::GetSelectedEntities() {
	std::vector<GameEntity*> res;
	const auto& selectedGuids = ImGuiSelection::GetSelectedObjects();
	
	for (const auto& guid : selectedGuids) {
		if (!guid.CheckValid()) continue;
		
		GameEntity* entity = GetSelectedEntity(guid);
		if (entity) res.push_back(entity);
	}
	
	// 最後に選択したものがインスペクタの基準になるように順序を調整（オプション）
	const Guid& lastGuid = ImGuiSelection::GetLastSelectedObject();
	std::sort(res.begin(), res.end(), [&](GameEntity* a, GameEntity* b) {
		if (a->GetGuid() == lastGuid) return true;
		if (b->GetGuid() == lastGuid) return false;
		return a->GetId() < b->GetId();
	});

	return res;
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

	// GetECSGroupsに入っていない動的なグループ（Debug用など）も明示的にチェック
	if (auto debugGroup = pEcs_->GetECSGroup("Debug")) {
		res = debugGroup->GetEntityFromGuid(entityGuid);
		if (res) return res;
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

void InspectorWindow::ShowMultiEntityBasicInfo(const std::vector<ONEngine::GameEntity*>& entities) {
	// 名前
	bool allSameName = true;
	std::string firstName = entities[0]->GetName();
	for (size_t i = 1; i < entities.size(); ++i) {
		if (entities[i]->GetName() != firstName) {
			allSameName = false;
			break;
		}
	}
	
	std::string nameLabel = allSameName ? firstName : "Mixed...";
	ImGuiInputTextReadOnly("entity names", nameLabel);

	// アクティブフラグ
	bool allSameActive = true;
	bool firstActive = entities[0]->active;
	for (size_t i = 1; i < entities.size(); ++i) {
		if (entities[i]->active != firstActive) {
			allSameActive = false;
			break;
		}
	}

	bool active = firstActive;
	if (allSameActive) {
		if (ImMathf::Checkbox("entity active", &active)) {
			for (auto e : entities) e->active = active;
		}
	} else {
		// Mixed state
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		if (ImGui::Button("Mixed Active (Set All True)")) {
			for (auto e : entities) e->active = true;
		}
		ImGui::PopStyleColor();
	}
}

///
/// エンティティのコンポーネントを表示する
///  
void InspectorWindow::ShowEntityComponents(ONEngine::GameEntity* entity) {
	for(auto itr = entity->GetComponents().begin(); itr != entity->GetComponents().end(); ) {
		DrawComponentNode(entity, itr);
	}
}

void InspectorWindow::ShowMultiEntityComponents(const std::vector<ONEngine::GameEntity*>& entities) {
	// 共通コンポーネントの抽出
	if (entities.empty()) return;

	std::map<size_t, std::vector<IComponent*>> commonComponents;
	
	// 初番目のエンティティのコンポーネントをベースにする
	for (auto& [hash, comp] : entities[0]->GetComponents()) {
		commonComponents[hash].push_back(comp);
	}

	// 2番目以降のエンティティと比較して、共通でないものを削除
	for (size_t i = 1; i < entities.size(); ++i) {
		auto& comps = entities[i]->GetComponents();
		for (auto it = commonComponents.begin(); it != commonComponents.end(); ) {
			auto compItr = comps.find(it->first);
			if (compItr != comps.end()) {
				it->second.push_back(compItr->second);
				++it;
			} else {
				it = commonComponents.erase(it);
			}
		}
	}

	// 共通コンポーネントの描画
	for (auto& [hash, comps] : commonComponents) {
		DrawMultiComponentNode(entities, hash, comps);
	}
}

///
/// エンティティに対してコンポーネントを追加するためのポップアップ
///
void InspectorWindow::ShowAddComponentPopup(ONEngine::GameEntity* entity) {
	std::vector<GameEntity*> entities = { entity };
	ShowMultiAddComponentPopup(entities);
}

void InspectorWindow::ShowMultiAddComponentPopup(const std::vector<ONEngine::GameEntity*>& entities) {
	if (entities.empty()) return;

	ImGui::Separator();

	const float indentSize = 4 * ImGui::GetStyle().IndentSpacing;
	ImGui::Indent(indentSize);

	const ImVec2 openPopupButtonSize = ImVec2(256.0f, 32.0f);
	if(ImGui::Button("Add Component", openPopupButtonSize)) {
		ImGui::OpenPopup("AddComponent");
	}

	ImGui::Unindent(indentSize);

	if(ImGui::BeginPopup("AddComponent")) {

		static char searchBuffer[256] = ""; 

		if(ImGui::IsWindowAppearing()) {
			memset(searchBuffer, 0, sizeof(searchBuffer));
			ImGui::SetKeyboardFocusHere();
		}

		ImGui::InputTextWithHint("##SearchComp", "Search Component...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
		ImGui::Separator();

		std::string searchStr = searchBuffer;
		std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), [](unsigned char c) { return std::tolower(c); });
		bool isSearching = !searchStr.empty();

		std::map<ComponentType, std::vector<std::string>> categorizedComponents;
		for(const auto& uiBinding : componentUIBindings_) {
			categorizedComponents[uiBinding.second.type].push_back(uiBinding.second.name);
		}

		for(auto& [type, names] : categorizedComponents) {
			std::sort(names.begin(), names.end());

			if(isSearching) {
				for(const auto& name : names) {
					std::string lowerName = name;
					std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char c) { return std::tolower(c); });

					if(lowerName.find(searchStr) != std::string::npos) {
						if(ImGui::MenuItem(name.c_str())) {
							for (auto e : entities) {
								pEditorManager_->ExecuteCommand<AddComponentCommand>(e, name);
							}
							ImGui::CloseCurrentPopup();
						}
					}
				}
			} else {
				std::string typeName{ magic_enum::enum_name(type) };
				if(ImGui::BeginMenu(typeName.c_str())) {
					for(const auto& name : names) {
						if(ImGui::MenuItem(name.c_str())) {
							for (auto e : entities) {
								pEditorManager_->ExecuteCommand<AddComponentCommand>(e, name);
							}
							ImGui::CloseCurrentPopup();
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

void InspectorWindow::DrawMultiComponentNode(const std::vector<ONEngine::GameEntity*>& entities, size_t hash, const std::vector<ONEngine::IComponent*>& comps) {
	std::string compName = componentUIBindings_.contains(hash) ? componentUIBindings_[hash].name : "Unknown Component";
	std::string label = compName + "##Multi" + std::to_string(hash);

	ImGui::PushID(label.c_str());

	ComponentType compType = componentUIBindings_.contains(hash) ? componentUIBindings_[hash].type : ComponentType::Compute;
	ImVec4 baseColor = GetComponentBaseColor(compType);

	bool isHeaderOpen = DrawMultiComponentHeaderUI(comps, compName, baseColor);

	bool isDeleted = false;
	if (isHeaderOpen) {
		isDeleted = HandleMultiComponentPopupMenu(entities, hash, compName);
		
		if (!isDeleted) {
			// 全員有効かチェック
			bool allEnabled = true;
			for (auto c : comps) if (!c->enable) { allEnabled = false; break; }
			
			DrawMultiComponentInnerContent(comps, hash, allEnabled);
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
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

bool InspectorWindow::DrawMultiComponentHeaderUI(const std::vector<ONEngine::IComponent*>& comps, const std::string& compName, ImVec4 baseColor) {
	ImGui::PushStyleColor(ImGuiCol_Header, baseColor);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(baseColor.x + 0.05f, baseColor.y + 0.05f, baseColor.z + 0.05f, 0.8f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
	bool isHeaderOpen = ImGui::TreeNodeEx("##header", flags, "");

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2.0f);

	bool allEnabled = true;
	bool firstEnabled = comps[0]->enable;
	for (auto c : comps) if (static_cast<bool>(c->enable) != firstEnabled) { allEnabled = false; break; }

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1, 1, 1, 0.1f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1, 1, 1, 0.2f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

	bool enabled = firstEnabled;
	if (allEnabled) {
		if (ImGui::Checkbox("##enabled", &enabled)) {
			for (auto c : comps) c->enable = enabled;
		}
	} else {
		if (ImGui::Button("-##mixed_enabled", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()))) {
			for (auto c : comps) c->enable = true;
		}
	}
	ImGui::PopStyleColor(4);

	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	ImGui::SameLine();
	ImGui::TextUnformatted(compName.c_str());

	float button_size = ImGui::GetFrameHeight();
	ImGui::SameLine(ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - button_size - 4.0f);
	if(ImGui::Button("::", ImVec2(button_size, button_size))) {
		ImGui::OpenPopup("MultiCompPopup");
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

bool InspectorWindow::HandleMultiComponentPopupMenu(const std::vector<ONEngine::GameEntity*>& entities, size_t hash, const std::string& compName) {
	bool isDeleted = false;

	if(ImGui::BeginPopup("MultiCompPopup")) {
		if(ImGui::MenuItem("Remove Component from all")) {
			for (auto e : entities) {
				auto comps = e->GetComponents();
				if (comps.contains(hash)) {
					pEditorManager_->ExecuteCommand<RemoveComponentCommand>(e, compName, nullptr);
				}
			}
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
		std::vector<IComponent*> comps = { comp };
		componentUIBindings_[componentTypeId].function(comps);
	}

	if(!enabled) ImGui::EndDisabled();
	ImGui::Unindent(22.0f);
}

void InspectorWindow::DrawMultiComponentInnerContent(const std::vector<ONEngine::IComponent*>& comps, size_t componentTypeId, bool enabled) {
	ImGui::Indent(22.0f);
	if(!enabled) ImGui::BeginDisabled();

	if(componentUIBindings_.contains(componentTypeId)) {
		componentUIBindings_[componentTypeId].function(comps);
	}

	if(!enabled) ImGui::EndDisabled();
	ImGui::Unindent(22.0f);
}

void InspectorWindow::AssetInspector() {
	/// Typeごとに表示を変える

	const Guid& selectionGuid = ImGuiSelection::GetLastSelectedObject();
	Asset::AssetType type = pAssetCollection_->GetAssetTypeFromGuid(selectionGuid);

	switch(type) {
	case Asset::AssetType::Texture:
	{
		ImGui::Text("Texture Inspector");
		ONEngine::Asset::Texture* texture = pAssetCollection_->GetTextureFromGuid(selectionGuid);
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
	if(tex->IsStandard2D()) {
		ImGui::Image((ImTextureID)(uintptr_t)tex->GetSRVGPUHandle().ptr, displaySize);
	} else {
		ImGui::Text("Preview not supported\n(CubeMap or 3D Texture)");
	}
	ImGui::EndChild();
}

} /// namespace Editor
