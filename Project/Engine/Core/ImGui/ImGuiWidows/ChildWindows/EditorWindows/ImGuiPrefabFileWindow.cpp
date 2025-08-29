#include "ImGuiPrefabFileWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "Engine/Graphics/Resource/GraphicsResourceCollection.h"
#include "ImGuiPrefabInspectorWindow.h"
#include "Engine/Script/MonoScriptEngine.h"

ImGuiPrefabFileWindow::ImGuiPrefabFileWindow(EntityComponentSystem* _ecs, GraphicsResourceCollection* _resourceCollection, ImGuiPrefabInspectorWindow* _inspector)
	: pECS_(_ecs), pResourceCollection_(_resourceCollection), pInspector_(_inspector) {

	files_ = Mathf::FindFiles("Assets/Prefabs", ".prefab");
}


void ImGuiPrefabFileWindow::ImGuiFunc() {
	if (!ImGui::Begin("Prefab File")) {
		ImGui::End();
		return;
	}


	const auto& textures = pResourceCollection_->GetTextures();
	const Texture& button = textures[pResourceCollection_->GetTextureIndex("./Packages/Textures/ImGui/reload.png")];

	//AddPrefabButton();
	//ImGui::SameLine();
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

			//GameEntity* entity = pECS_->GeneratePrefabEntity(file.second); // Prefabを生成する関数を呼び出す
			//pInspector_->SetSelectedEntity(reinterpret_cast<std::uintptr_t>(entity));
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
			//pECS_->ReloadPrefab(file.second);
		}

	}

}

void ImGuiPrefabFileWindow::AddPrefabButton() {

	if (ImGui::Button("+")) {
		ImGui::OpenPopup("add prefab popup");
	}


	if (ImGui::BeginPopup("add prefab popup")) {

		/// 新規Prefabの名前入力
		if (ImMathf::InputText("new prefab name", &newPrefabName_, ImGuiInputTextFlags_EnterReturnsTrue)) {

			if (!newPrefabName_.empty()) {
				/// 拡張子を追加
				if (newPrefabName_.find(".prefab") == std::string::npos) {
					newPrefabName_ += ".prefab";
				}

				/// 先にPrefabをリロード
				//pECS_->ReloadPrefab(newPrefabName_);

				/// Prefabを生成
				//GameEntity* entity = pECS_->GeneratePrefabEntity(newPrefabName_);
				//if (entity) {
				//	Console::Log("Prefab created: " + newPrefabName_);
				//	pInspector_->SetSelectedEntity(reinterpret_cast<std::uintptr_t>(entity));
				//} else {
				//	Console::LogError("Failed to create prefab: " + newPrefabName_);
				//}
				newPrefabName_.clear(); // 入力フィールドをクリア
			}
		}

		ImGui::EndPopup();

	}

}
