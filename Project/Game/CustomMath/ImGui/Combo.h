#pragma once

/// std
#include <vector>
#include <string>

/// externals
#include <imgui.h>

/// <summary>
/// imgui
/// </summary>
/// <param name="_index"></param>
/// <param name="_strings"></param>
void ImGuiCombo(int& _index, const std::vector<std::string>& _strings) {

	/// const char*の配列に変更する
	std::vector<const char*> itemCStrs;
	for(const auto& item : _strings) {
		itemCStrs.push_back(item.c_str());
	}

	/// Comboを描画
	ImGui::Combo("Select an option", &_index, itemCStrs.data(), static_cast<int>(itemCStrs.size()));
}