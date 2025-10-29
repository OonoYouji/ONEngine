#include "ImGuiCommand.h"

/// externals
#include <imgui.h>

/// engine
#include "Engine/Editor/EditCommand.h"

bool ImMath::DragFloat(const std::string& _label, float* _pv, float _step, float _min, float _max) {
	static float startValue{};

	bool edit = ImGui::DragFloat(_label.c_str(), _pv, _step, _min, _max);
	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		float endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::FloatCommand>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMath::DragFloat3(const std::string& _label, Vector3* _pv, float _step, float _min, float _max) {
	static Vector3 startValue{};

	bool edit = ImGui::DragFloat3(_label.c_str(), &_pv->x, _step, _min, _max);

	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		Vector3 endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::Vec3Command>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMath::DragFloat4(const std::string& _label, Vector4* _pv, float _step, float _min, float _max) {
	static Vector4 startValue{};

	bool edit = ImGui::DragFloat3(_label.c_str(), &_pv->x, _step, _min, _max);

	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pv;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		Vector4 endValue = *_pv;
		EditCommand::Execute<ImGuiCommand::Vec4Command>(_pv, startValue, endValue);
	}

	return edit;
}

bool ImMath::DragQuaternion(const std::string& _label, Quaternion* _pq, float _step, float _min, float _max) {
	static Quaternion startValue{};

	/// Eulerに変換して表示
	Vector3 euler = Quaternion::ToEuler(*_pq);
	bool edit = ImGui::DragFloat3(_label.c_str(), &euler.x, _step, _min, _max);
	if (edit) {
		*_pq = Quaternion::FromEuler(euler);
	}

	/// 操作を始めた
	if (ImGui::IsItemActivated()) {
		startValue = *_pq;
	}

	/// 操作を終えた
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		Quaternion endValue = *_pq;
		EditCommand::Execute<ImGuiCommand::QuaternionCommand>(_pq, startValue, endValue);
	}

	return false;
}


/// ///////////////////////////////////////////////////
/// ImGuiで操作したFloatのRedo,Undoコマンド
/// ///////////////////////////////////////////////////
ImGuiCommand::FloatCommand::FloatCommand(float* _v, float _old, float _new)
	: pValue_(_v), oldValue_(_old), newValue_(_new) {
}

EDITOR_STATE ImGuiCommand::FloatCommand::Execute() {
	if (pValue_) {
		*pValue_ = newValue_;
	} else {
		Console::Log("ImGuiCommand::FloatCommand : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE ImGuiCommand::FloatCommand::Undo() {
	if (pValue_) {
		*pValue_ = oldValue_;
	} else {
		Console::Log("ImGuiCommand::FloatCommand : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}


/// ///////////////////////////////////////////////////
/// ImGuiで操作したVector3のRedo,Undoコマンド
/// ///////////////////////////////////////////////////
ImGuiCommand::Vec3Command::Vec3Command(Vector3* _v, const Vector3& _old, const Vector3& _new)
	: pValue_(_v), oldValue_(_old), newValue_(_new) {
}

EDITOR_STATE ImGuiCommand::Vec3Command::Execute() {
	if (pValue_) {
		*pValue_ = newValue_;
	} else {
		Console::Log("ImGuiCommand::Vec3Command : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE ImGuiCommand::Vec3Command::Undo() {
	if (pValue_) {
		*pValue_ = oldValue_;
	} else {
		Console::Log("ImGuiCommand::Vec3Command : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

/// ///////////////////////////////////////////////////
/// ImGuiで操作したVector4のRedo,Undoコマンド
/// ///////////////////////////////////////////////////
ImGuiCommand::Vec4Command::Vec4Command(Vector4* _v, const Vector4& _old, const Vector4& _new)
	: pValue_(_v), oldValue_(_old), newValue_(_new) {
}

EDITOR_STATE ImGuiCommand::Vec4Command::Execute() {
	if (pValue_) {
		*pValue_ = newValue_;
	} else {
		Console::Log("ImGuiCommand::Vec4Command : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE ImGuiCommand::Vec4Command::Undo() {
	if (pValue_) {
		*pValue_ = oldValue_;
	} else {
		Console::Log("ImGuiCommand::Vec4Command : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

/// ///////////////////////////////////////////////////
/// ImGuiで操作したQuaternionのRedo,Undoコマンド
/// ///////////////////////////////////////////////////
ImGuiCommand::QuaternionCommand::QuaternionCommand(Quaternion* _q, const Quaternion& _old, const Quaternion& _new)
	: pValue_(_q), oldValue_(_old), newValue_(_new) {
}

EDITOR_STATE ImGuiCommand::QuaternionCommand::Execute() {
	if (pValue_) {
		*pValue_ = newValue_;
	} else {
		Console::Log("ImGuiCommand::QuaternionCommand : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

EDITOR_STATE ImGuiCommand::QuaternionCommand::Undo() {
	if (pValue_) {
		*pValue_ = oldValue_;
	} else {
		Console::Log("ImGuiCommand::QuaternionCommand : Value is nullptr");
		return EDITOR_STATE::EDITOR_STATE_FAILED;
	}
	return EDITOR_STATE::EDITOR_STATE_FINISH;
}

