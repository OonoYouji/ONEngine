#pragma once

/// std
#include <string>
#include <type_traits>
#include <algorithm>

/// externals
#include <magic_enum/magic_enum.hpp>
#include <imgui.h>

/// editor
#include "Engine/Editor/Manager/EditCommand.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"

namespace Editor {

template <typename T>
bool Combo(const std::string& label, T& value, float columnWidth = 100.0f) {
	static_assert(std::is_enum_v<T>, "T must be an enum type");

	constexpr auto values = magic_enum::enum_values<T>();
	constexpr auto names = magic_enum::enum_names<T>();

	// 現在のインデックスを検索
	int current = 0;
	for(size_t i = 0; i < values.size(); ++i) {
		if(values[i] == value) {
			current = static_cast<int>(i);
			break;
		}
	}

	bool changed = false;

	// --- レイアウト開始 ---
	ImGui::PushID(label.c_str());
	ImGuiTableFlags tableFlags = ImGuiTableFlags_NoSavedSettings;

	// 2列のテーブルを作成 (Label | Value)
	if(ImGui::BeginTable("##ComboTable", 2, tableFlags)) {
		// 列幅の設定
		ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, columnWidth);
		ImGui::TableSetupColumn("##Value", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow();

		// 1列目: ラベル
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding(); // テキストを垂直方向中央揃え
		ImGui::Text("%s", label.c_str());

		// 2列目: コンボボックス
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x); // 横幅いっぱいに広げる

		using NamesType = decltype(names);

		T preValue = value;
		if(ImGui::Combo(
			"##v", &current,
			[](void* data, int index, const char** out_text) -> bool {
			auto& names = *static_cast<NamesType*>(data);
			*out_text = names[index].data();
			return true; }, 
			(void*)&names, static_cast<int>(names.size()))) {

			changed = true;
			value = values[current];

			// 変更があった瞬間にコマンドを発行
			if(preValue != value) {
				EditCommand::Execute<ModifyValueCommand<T>>(&value, preValue, value);
			}
		}

		ImGui::EndTable();
	}
	ImGui::PopID();

	return changed;
}

} // namespace Editor