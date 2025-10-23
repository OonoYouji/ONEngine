#include "ImGuiPrefabFileWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Asset/Collection/AssetCollection.h"
#include "../GameWindows/ImGuiInspectorWindow.h"
#include "Engine/Script/MonoScriptEngine.h"

ImGuiPrefabFileWindow::ImGuiPrefabFileWindow(EntityComponentSystem* _ecs, AssetCollection* _assetCollection, ImGuiInspectorWindow* _inspector)
	: pEcs_(_ecs), pAssetCollection_(_assetCollection), pInspector_(_inspector) {

	files_ = Mathf::FindFiles("Assets/Prefabs", ".prefab");
}


void ImGuiPrefabFileWindow::ShowImGui() {
	if (!ImGui::Begin("Prefab File")) {
		ImGui::End();
		return;
	}

	const auto& textures = pAssetCollection_->GetTextures();
	const Texture& button = textures[pAssetCollection_->GetTextureIndex("./Packages/Textures/ImGui/reload.png")];

	ReloadPrefabFiles(&button);

	ImGui::Separator();

	/// fileの表示
	ImGuiInputText("search prefab", &searchText_, ImGuiInputTextFlags_EnterReturnsTrue);

	for (auto& file : files_) {
		/// 検索ボックスに入力されたテキストがファイル名に含まれているかチェック
		size_t size = searchText_.size();
		if (size != 0) {
			if (file.second.find(searchText_) == std::string::npos) {
				continue; // 検索テキストが含まれていない場合はスキップ
			}
		}

		ImGui::Selectable(file.second.c_str());
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			Console::Log("Double clicked prefab file: " + file.second);

			ECSGroup* debugGroup = pEcs_->GetECSGroup("Debug");
			GameEntity* entity = debugGroup->GenerateEntityFromPrefab(file.second, false);
			pInspector_->SetSelectedEntity(entity);
		}

	}


	ImGui::End();
}

void ImGuiPrefabFileWindow::ReloadPrefabFiles(const Texture* _tex) {

	/// Reloadボタンの表示
	ImVec2 buttonSize = ImVec2(24.0f, 24.0f);
	if (ImGui::ImageButton(
		"##reload", ImTextureID(_tex->GetSRVGPUHandle().ptr), buttonSize,
		ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 0), ImVec4(0.1f, 0.1f, 0.75f, 1))) {

		/// ファイルの再読み込み
		files_ = Mathf::FindFiles("Assets/Prefabs", ".prefab");

		for (auto& file : files_) {
			/// ファイル名の置換
			pEcs_->ReloadPrefab(file.second);
		}

	}

}

