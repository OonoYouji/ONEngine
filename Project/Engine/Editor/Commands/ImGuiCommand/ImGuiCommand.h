#pragma once

/// std
#include <string>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "../Interface/IEditorCommand.h"

/// ///////////////////////////////////////////////////
/// ImGui関連の編集
/// ///////////////////////////////////////////////////
namespace ImMath {

	bool DragFloat3(const std::string& _label, Vector3* _pv, float _step = 1.0f, float _min = 0.0f, float _max = 0.0f);
	bool DragQuaternion(const std::string& _label, Quaternion* _pq, float _step = 0.1f, float _min = 0.0f, float _max = 0.0f);
}


namespace ImGuiCommand {

	/// ///////////////////////////////////////////////////
	/// ImGuiで操作したVector3のRedo,Undoコマンド
	/// ///////////////////////////////////////////////////
	class Vec3Command : public IEditorCommand {
	public:
		Vec3Command(Vector3* _v, const Vector3& _old, const Vector3& _new);
		~Vec3Command() override = default;
		EDITOR_STATE Execute() override;
		EDITOR_STATE Undo() override;
	private:
		Vector3* pValue_;
		Vector3 oldValue_, newValue_;
	};

	/// ///////////////////////////////////////////////////
	/// ImGuiで操作したQuaternionのRedo,Undoコマンド
	/// ///////////////////////////////////////////////////
	class QuaternionCommand : public IEditorCommand {
	public:
		QuaternionCommand(Quaternion* _q, const Quaternion& _old, const Quaternion& _new);
		~QuaternionCommand() override = default;
		EDITOR_STATE Execute() override;
		EDITOR_STATE Undo() override;
	private:
		Quaternion* pValue_;
		Quaternion oldValue_, newValue_;
	};

}
