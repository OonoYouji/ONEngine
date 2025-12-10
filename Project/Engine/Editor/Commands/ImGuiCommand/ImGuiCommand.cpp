#include "ImGuiCommand.h"


/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/Manager/EditCommand.h"

using namespace Editor;

bool ImMathf::DragInt(const std::string& _label, int* _pv, int _step, int _min, int _max) {
	static int startValue{};

	bool edit = ImGui::DragInt(_label.c_str(), _pv, static_cast<float>(_step), _min, _max);
	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		int endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<int>>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMathf::DragInt2(const std::string& _label, ONEngine::Vector2Int* _pv, int _step, int _min, int _max) {
	static ONEngine::Vector2Int startValue{};

	bool edit = ImGui::DragInt2(_label.c_str(), &_pv->x, static_cast<float>(_step), _min, _max);
	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		ONEngine::Vector2Int endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<ONEngine::Vector2Int>>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMathf::DragInt3(const std::string& _label, ONEngine::Vector3Int* _pv, int _step, int _min, int _max) {
	static ONEngine::Vector3Int startValue{};

	bool edit = ImGui::DragInt3(_label.c_str(), &_pv->x, static_cast<float>(_step), _min, _max);
	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		ONEngine::Vector3Int endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<ONEngine::Vector3Int>>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMathf::DragFloat(const std::string& _label, float* _pv, float _step, float _min, float _max, const char* _format) {
	static float startValue{};

	bool edit = ImGui::DragFloat(_label.c_str(), _pv, _step, _min, _max, _format);
	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		float endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<float>>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMathf::DragFloat2(const std::string& _label, ONEngine::Vector2* _pv, float _step, float _min, float _max) {
	static ONEngine::Vector2 startValue{};

	bool edit = ImGui::DragFloat2(_label.c_str(), &_pv->x, _step, _min, _max);
	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		ONEngine::Vector2 endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<ONEngine::Vector2>>(_pv, startValue, endValue);
	}

	return edit;
}


bool ImMathf::DragFloat3(const std::string& _label, ONEngine::Vector3* _pv, float _step, float _min, float _max) {
	static ONEngine::Vector3 startValue{};

	bool edit = ImGui::DragFloat3(_label.c_str(), &_pv->x, _step, _min, _max);

	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		ONEngine::Vector3 endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<ONEngine::Vector3>>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMathf::DragFloat4(const std::string& _label, ONEngine::Vector4* _pv, float _step, float _min, float _max) {
	static ONEngine::Vector4 startValue{};

	bool edit = ImGui::DragFloat4(_label.c_str(), &_pv->x, _step, _min, _max);

	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		ONEngine::Vector4 endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<ONEngine::Vector4>>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMathf::DragQuaternion(const std::string& _label, ONEngine::Quaternion* _pq, float _step, float _min, float _max) {
	static ONEngine::Quaternion startValue{};

	/// Eulerに変換して表示
	ONEngine::Vector3 euler = ONEngine::Quaternion::ToEuler(*_pq);
	bool edit = ImGui::DragFloat3(_label.c_str(), &euler.x, _step, _min, _max);
	if (edit) {
		*_pq = ONEngine::Quaternion::FromEuler(euler);
	}

	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pq;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		ONEngine::Quaternion endValue = *_pq;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<ONEngine::Quaternion>>(_pq, startValue, endValue);
	}

	return false;
}

bool ImMathf::Checkbox(const std::string& _label, bool* _pv) {

	static bool startValue{};

	startValue = *_pv;
	bool edit = ImGui::Checkbox(_label.c_str(), _pv);
	if (edit) {
		bool endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::ModifyValueCommand<bool>>(_pv, startValue, endValue);
	}


	return edit;
}

