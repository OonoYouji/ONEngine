#pragma once

/// std
#include <string>

/// engine
#include "Engine/Core/Utility/Utility.h"
#include "../Interface/IEditorCommand.h"

namespace ONEngine {

/// ///////////////////////////////////////////////////
/// ImGui関連の編集
/// ///////////////////////////////////////////////////
namespace ImMathf {

	/// @brief ImGuiのDragIntでintを操作するコマンド
	/// @param _label DragIntのラベル
	/// @param _pv intのポインタ
	/// @param _step 1回の操作で変化する値
	/// @param _min _pvの最小値
	/// @param _max _pvの最大値
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool DragInt(const std::string& _label, int* _pv, int _step = 1, int _min = 0, int _max = 0);

	/// @brief ImGuiのDragInt2でVector2Intを操作するコマンド
	/// @param _label DragInt2のラベル
	/// @param _pv Vector2Intのポインタ
	/// @param _step 1回の操作で変化する値
	/// @param _min _pvの最小値
	/// @param _max _pvの最大値
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool DragInt2(const std::string& _label, Vector2Int* _pv, int _step = 1, int _min = 0, int _max = 0);

	/// @brief ImGuiのDragInt3でVector3Intを操作するコマンド
	/// @param _label DragInt3のラベル
	/// @param _pv Vector3Intのポインタ
	/// @param _step 1回の操作で変化する値
	/// @param _min _pvの最小値
	/// @param _max _pvの最大値
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool DragInt3(const std::string& _label, Vector3Int* _pv, int _step = 1, int _min = 0, int _max = 0);

	/// @brief ImGuiのDragFloatでfloatを操作するコマンド
	/// @param _label DragFloatのラベル
	/// @param _pv floatのポインタ
	/// @param _step 1回の操作で変化する値
	/// @param _min _pvの最小値
	/// @param _max _pvの最大値
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool DragFloat(const std::string& _label, float* _pv, float _step = 1.0f, float _min = 0.0f, float _max = 0.0f, const char* _format = "%.3f");

	/// @brief ImGuiのDragFloat2でVector2を操作するコマンド
	/// @param _label DragFloat2のラベル
	/// @param _pv floatのポインタ
	/// @param _step 1回の操作で変化する値
	/// @param _min _pvの最小値
	/// @param _max _pvの最大値
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool DragFloat2(const std::string& _label, Vector2* _pv, float _step = 1.0f, float _min = 0.0f, float _max = 0.0f);

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


	/// @brief フラグを切り替えるチェックボックスを表示します。
	/// @param _label UI上で表示するラベル文字列。
	/// @param _pv 調整対象となるブール値へのポインタ。
	/// @return true: 値が変更された, false: 値が変更されなかった
	bool Checkbox(const std::string& _label, bool* _pv);

}



/// ///////////////////////////////////////////////////
/// ImGuiCommand名前空間
/// ///////////////////////////////////////////////////
namespace ImGuiCommand {

	/// @brief T型の値を変更するコマンド
	/// @tparam T 変更する値の型 (T型のoperator=が定義されている必要がある)
	template <typename T>
	class ModifyValueCommand : public IEditorCommand {
	public:
		/// ===================================================
		/// public : methods
		/// ===================================================

		ModifyValueCommand(T* _v, const T& _old, const T& _new)
			: pValue_(_v), oldValue_(_old), newValue_(_new) {
		}
		~ModifyValueCommand() = default;
		EDITOR_STATE Execute() {
			if (pValue_) {
				*pValue_ = newValue_;
			} else {
				Console::LogError("ImGuiCommand::ModifyValueCommand : Value is nullptr");
				return EDITOR_STATE::EDITOR_STATE_FAILED;
			}
			return EDITOR_STATE::EDITOR_STATE_FINISH;
		}
		EDITOR_STATE Undo() {
			if (pValue_) {
				*pValue_ = oldValue_;
			} else {
				Console::LogError("ImGuiCommand::ModifyValueCommand : Value is nullptr");
				return EDITOR_STATE::EDITOR_STATE_FAILED;
			}
			return EDITOR_STATE::EDITOR_STATE_FINISH;
		}
	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		T* pValue_;
		T oldValue_, newValue_;
	};

}

} /// ONEngine
