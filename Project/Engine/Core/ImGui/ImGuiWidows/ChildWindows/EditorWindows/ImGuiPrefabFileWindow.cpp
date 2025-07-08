#include "ImGuiPrefabFileWindow.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/Math/ImGuiMath.h"
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"
#include "ImGuiPrefabInspectorWindow.h"

ImGuiPrefabFileWindow::ImGuiPrefabFileWindow(EntityComponentSystem* _ecs, ImGuiPrefabInspectorWindow* _inspector)
	: pECS_(_ecs), pInspector_(_inspector) {

	files_ = Mathf::FindFiles("Assets/Prefabs", ".prefab");
}

void ImGuiPrefabFileWindow::ImGuiFunc() {
	if (!ImGui::Begin("Prefab File")) {
		ImGui::End();
		return;
	}


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
			// ここでPrefabをロードする処理を追加することができます

			IEntity* entity = pECS_->GeneratePrefabEntity(file.second); // Prefabを生成する関数を呼び出す
			pInspector_->SetSelectedEntity(reinterpret_cast<std::uintptr_t>(entity));
		}

	}


	ImGui::End();
}
