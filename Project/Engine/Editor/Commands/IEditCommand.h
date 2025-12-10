#pragma once

/// std
#include <memory>

/// ///////////////////////////////////////////////////
/// エディタのコマンドの状態
/// ///////////////////////////////////////////////////
enum EDITOR_STATE {
	 EDITOR_STATE_RUNNING,
	 EDITOR_STATE_FINISH,
	 EDITOR_STATE_FAILED,
};


/// ///////////////////////////////////////////////////
/// Editorのコマンドのインターフェース
/// ///////////////////////////////////////////////////
namespace ONEngine {

class IEditCommand {
public:
	/// ==========================================
	/// public : methods
	/// ==========================================

	IEditCommand() = default;
	virtual ~IEditCommand() = default;

	/// @brief コマンドの実行
	/// @return 現在の状態
	virtual EDITOR_STATE Execute() = 0;

	/// @brief コマンドの取り消し操作
	/// @return 
	virtual EDITOR_STATE Undo() = 0;

};


} /// ONEngine
