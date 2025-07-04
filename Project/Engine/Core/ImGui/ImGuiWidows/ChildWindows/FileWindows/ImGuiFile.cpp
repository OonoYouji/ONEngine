#include "ImGuiFile.h"

/// std
#include <filesystem>

/// external
#include <imgui.h>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Core/ImGui/ImGuiManager.h"


ImGuiFile::ImGuiFile() {}

void ImGuiFile::ImGuiFunc() {
	if (!ImGui::Begin("ImGui")) {
		ImGui::End();
		return;
	}


	/// imguiのstyleを出力
	if (ImGui::CollapsingHeader("imgui style")) {

		if (ImGui::Button("save style")) {
			/// すでに"imgui_style.json"があればpopup windowを表示して警告
			const std::string&& fileName = "./imgui_style.json";
			if (std::filesystem::exists(fileName)) {
				ImGui::OpenPopup("warring!!!");
			} else {
				pImGuiManager_->OutputImGuiStyle("./imgui_style.json");
			}
		}

		ImGui::SeparatorText("style editor");

		ImGui::ShowStyleEditor();
	}


	if (ImGui::BeginPopupContextItem("warring!!!")) {
		ImGui::PushStyleColor(0, ImVec4(1,1,1,1));

		/// textで警告内容を表示
		ImGui::Text("\"imgui_style.json\"がすでに存在します、上書きしますか？");


		if (ImGui::Button("ok")) {
			pImGuiManager_->OutputImGuiStyle("./imgui_style.json");
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();

		if (ImGui::Button("cancel")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopStyleColor(1);
		ImGui::EndPopup();
	}

	ImGui::End();
}
