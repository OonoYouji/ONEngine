#include "ImGuiPrefabFileWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Script/MonoScriptEngine.h"

/// engine/imgui
#include "Engine/Core/ImGui/ImGuiSelection.h"
#include "../GameWindows/ImGuiInspectorWindow.h"


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

	ImGui::SameLine();
	ImGui::Spacing();
	ImGui::SameLine();

	AddNewPrefabWindow();

	ImGui::Separator();

	/// fileの表示
	ImGuiInputText("search prefab", &searchText_, ImGuiInputTextFlags_EnterReturnsTrue);
	ShowPrefabFileList();

	ImGui::End();
}

void ImGuiPrefabFileWindow::ShowPrefabFileList() {
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

			/// すでに生成されているなら削除してから再生成
			GameEntity* selectedEntity = pEcs_->GetECSGroup("Debug")->GetEntityFromGuid(selectedPrefabGuid_);
			if (selectedEntity) {
				selectedEntity->Destroy();
			}

			ECSGroup* debugGroup = pEcs_->GetECSGroup("Debug");
			selectedEntity = debugGroup->GenerateEntityFromPrefab(file.second, false);
			selectedPrefabGuid_ = selectedEntity->GetGuid();
			ImGuiSelection::SetSelectedObject(selectedPrefabGuid_, SelectionType::Entity);
		}

	}
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

void ImGuiPrefabFileWindow::AddNewPrefabWindow() {

	/// 新規作成ボタン
	if (ImGui::Button("New Prefab")) {
		ImGui::OpenPopup("New Prefab");
	}

	/// 新規作成ポップアップ
	if (ImGui::BeginPopup("New Prefab")) {
		ImMathf::InputText("Prefab Name", &newPrefabName_);
		if (ImGui::Button("Create")) {
			if (GenerateNewPrefab()) {
				ImGui::CloseCurrentPopup();
				newPrefabName_.clear();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
			newPrefabName_.clear();
		}
		ImGui::EndPopup();
	}

}

bool ImGuiPrefabFileWindow::GenerateNewPrefab() {

	/// 既に同じ名前のPrefabが存在するかチェック
	for (const auto& file : files_) {
		if (file.second == newPrefabName_) {
			Console::LogWarning("Prefab with the same name already exists: " + newPrefabName_);
			return false;
		}
	}

	/// Prefabの生成
	const std::string filename = "./Assets/Prefabs/" + newPrefabName_ + ".prefab";

	std::ofstream prefabFile(filename);
	if (!prefabFile) {
		Console::LogError("Failed to create prefab file: " + filename);
		return false;
	}

	nlohmann::json json;
	json["prefabName"] = newPrefabName_;

	prefabFile << json; // 空のJSONオブジェクトを初期内容として書き込む
	prefabFile.close();

	Console::Log("Prefab file created successfully: " + filename);

	pEcs_->ReloadPrefab(newPrefabName_ + ".prefab");
	/// ファイルリストを更新
	files_ = Mathf::FindFiles("Assets/Prefabs", ".prefab");

	return true;
}

