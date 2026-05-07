#include "BasicTypeWidget.h"

/// externals
#include <imgui.h>
#include <imgui_internal.h>

/// editor
#include "Engine/Editor/Manager/EditCommand.h"
#include "Engine/Editor/Commands/ImGuiCommand/ImGuiCommand.h"

using namespace Editor;

namespace {

// レイアウト開始
bool BeginPropertyRow(const std::string& _label, float _columnWidth) {
	ImGui::PushID(_label.c_str());
	ImGuiTableFlags tableFlags = ImGuiTableFlags_NoSavedSettings;
	if(ImGui::BeginTable("##PropertyTable", 2, tableFlags)) {
		ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, _columnWidth);
		ImGui::TableSetupColumn("##Value", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", _label.c_str());
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		return true;
	}
	return false;
}

// レイアウト終了
void EndPropertyRow() {
	ImGui::EndTable();
	ImGui::PopID();
}

// Undo/Redo処理用ヘルパー
// _pVal         : 変数へのポインタ
// _preVal       : ImGui関数を呼ぶ前の値
// _staticStartVal: 操作開始時の値を保持する静的変数の参照
template<typename T>
void HandleUndo(T* _pVal, const T& _preVal, T& _staticStartVal) {
	// アイテムがアクティブになった瞬間（クリックした瞬間など）
	// ImGuiの関数実行ですでに値が変わっている可能性があるため、
	// 事前に退避しておいた _preVal を開始値として保存する
	if(ImGui::IsItemActivated()) {
		_staticStartVal = _preVal;
	}

	// 編集終了後（マウスリリース、Enter確定など）
	if(ImGui::IsItemDeactivatedAfterEdit()) {
		// 値が実際に変わっていたらコマンド発行
		if(*_pVal != _staticStartVal) {
			EditCommand::Execute<ModifyValueCommand<T>>(_pVal, _staticStartVal, *_pVal);
		}
	}
}

} /// namespace


// ==================================================================================
// Int Implementation
// ==================================================================================

bool Editor::DragInt(const std::string& _label, int& _v, float _v_speed, int _v_min, int _v_max, const char* _format, ImGuiSliderFlags _flags, float _columnWidth) {
	bool changed = false;
	static int s_startVal = 0; // 操作開始時の値を保持

	if(BeginPropertyRow(_label, _columnWidth)) {
		int preVal = _v; // ImGui呼び出し前の値を保存

		if(ImGui::DragInt("##v", &_v, _v_speed, _v_min, _v_max, _format, _flags)) {
			changed = true;
		}

		// Undo処理
		HandleUndo(&_v, preVal, s_startVal);

		EndPropertyRow();
	}
	return changed;
}

bool Editor::SliderInt(const std::string& _label, int& _v, int _v_min, int _v_max, const char* _format, ImGuiSliderFlags _flags, float _columnWidth) {
	bool changed = false;
	static int s_startVal = 0;

	if(BeginPropertyRow(_label, _columnWidth)) {
		int preVal = _v;

		if(ImGui::SliderInt("##v", &_v, _v_min, _v_max, _format, _flags)) {
			changed = true;
		}

		HandleUndo(&_v, preVal, s_startVal);

		EndPropertyRow();
	}
	return changed;
}

bool Editor::InputInt(const std::string& _label, int& _v, int _step, int _step_fast, ImGuiInputTextFlags _flags, float _columnWidth) {
	bool changed = false;
	static int s_startVal = 0;

	if(BeginPropertyRow(_label, _columnWidth)) {
		int preVal = _v;

		if(ImGui::InputInt("##v", &_v, _step, _step_fast, _flags)) {
			changed = true;
		}

		HandleUndo(&_v, preVal, s_startVal);

		EndPropertyRow();
	}
	return changed;
}

// ==================================================================================
// Float Implementation
// ==================================================================================

bool Editor::DragFloat(const std::string& _label, float& _v, float _v_speed, float _v_min, float _v_max, const char* _format, ImGuiSliderFlags _flags, float _columnWidth) {
	bool changed = false;
	static float s_startVal = 0.0f;

	if(BeginPropertyRow(_label, _columnWidth)) {
		float preVal = _v;

		if(ImGui::DragFloat("##v", &_v, _v_speed, _v_min, _v_max, _format, _flags)) {
			changed = true;
		}

		HandleUndo(&_v, preVal, s_startVal);

		EndPropertyRow();
	}
	return changed;
}

bool Editor::SliderFloat(const std::string& _label, float& _v, float _v_min, float _v_max, const char* _format, ImGuiSliderFlags _flags, float _columnWidth) {
	bool changed = false;
	static float s_startVal = 0.0f;

	if(BeginPropertyRow(_label, _columnWidth)) {
		float preVal = _v;

		if(ImGui::SliderFloat("##v", &_v, _v_min, _v_max, _format, _flags)) {
			changed = true;
		}

		HandleUndo(&_v, preVal, s_startVal);

		EndPropertyRow();
	}
	return changed;
}

bool Editor::InputFloat(const std::string& _label, float& _v, float _step, float _step_fast, const char* _format, ImGuiInputTextFlags _flags, float _columnWidth) {
	bool changed = false;
	static float s_startVal = 0.0f;

	if(BeginPropertyRow(_label, _columnWidth)) {
		float preVal = _v;

		if(ImGui::InputFloat("##v", &_v, _step, _step_fast, _format, _flags)) {
			changed = true;
		}

		HandleUndo(&_v, preVal, s_startVal);

		EndPropertyRow();
	}
	return changed;
}