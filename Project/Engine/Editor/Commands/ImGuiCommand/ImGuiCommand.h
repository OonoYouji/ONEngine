#pragma once

/// std
#include <string>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "../Interface/IEditorCommand.h"

/// ///////////////////////////////////////////////////
/// ImGui関連の編集
/// ///////////////////////////////////////////////////
namespace ImMathf {


	/// @brief ImGuiのDragFloatでfloatを操作するコマンド
	/// @param _label DragFloatのラベル
	/// @param _pv floatのポインタ
	/// @param _step 1回の操作で変化する値
	/// @param _min _pvの最小値
	/// @param _max _pvの最大値
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool DragFloat(const std::string& _label, float* _pv, float _step = 1.0f, float _min = 0.0f, float _max = 0.0f, const char* _format = "%.3f");

	/// @brief ImGuiのDragFloat3でVector3を操作するコマンド
	/// @param _label DragFloat3のラベル
	/// @param _pv Vector3のポインタ
	/// @param _step 1回の操作で変化する値
	/// @param _min _pvの最小値
	/// @param _max _pvの最大値
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool DragFloat3(const std::string& _label, Vector3* _pv, float _step = 1.0f, float _min = 0.0f, float _max = 0.0f);

	/// @brief 4つの浮動小数点値をドラッグ操作で編集できるUIウィジェットを表示します。
	/// @param _label ウィジェットに表示するラベル文字列。
	/// @param _pv 編集対象となる4要素のベクトル（Vector4型）へのポインタ。
	/// @param _step ドラッグ時の増減ステップ値（デフォルトは1.0f）。
	/// @param _min 値の最小制限（デフォルトは0.0f、0の場合は制限なし）。
	/// @param _max 値の最大制限（デフォルトは0.0f、0の場合は制限なし）。
	/// @return 値が変更された場合はtrue、変更されなかった場合はfalse。
	bool DragFloat4(const std::string& _label, Vector4* _pv, float _step = 1.0f, float _min = 0.0f, float _max = 0.0f);

	/// @brief クォータニオンの値をEulerに変換しドラッグ操作で調整します。
	/// @param _label UI上で表示するラベル文字列。
	/// @param _pq 調整対象となるクォータニオンへのポインタ。
	/// @param _step ドラッグ時の増減ステップ値（デフォルトは0.1f）。
	/// @param _min 調整可能な最小値（デフォルトは0.0f、制限なし）。
	/// @param _max 調整可能な最大値（デフォルトは0.0f、制限なし）。
	/// @return 値が変更された場合はtrue、変更されなかった場合はfalse。
	bool DragQuaternion(const std::string& _label, Quaternion* _pq, float _step = 0.1f, float _min = 0.0f, float _max = 0.0f);
}



/// ///////////////////////////////////////////////////
/// ImGuiCommand名前空間
/// ///////////////////////////////////////////////////
namespace ImGuiCommand {


	/// ///////////////////////////////////////////////////
	/// ImGuiで操作したFloatのRedo,Undoコマンド
	/// ///////////////////////////////////////////////////
	class FloatCommand : public IEditorCommand {
	public:
		FloatCommand(float* _v, float _old, float _new);
		~FloatCommand() override = default;
		EDITOR_STATE Execute() override;
		EDITOR_STATE Undo() override;
	private:
		float* pValue_;
		float oldValue_, newValue_;
	};


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
	/// ImGuiで操作したVector4のRedo,Undoコマンド
	/// ///////////////////////////////////////////////////
	class Vec4Command : public IEditorCommand {
	public:
		Vec4Command(Vector4* _v, const Vector4& _old, const Vector4& _new);
		~Vec4Command() override = default;
		EDITOR_STATE Execute() override;
		EDITOR_STATE Undo() override;
	private:
		Vector4* pValue_;
		Vector4 oldValue_, newValue_;
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
