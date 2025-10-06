#pragma once

/// engine
#include "EditorManager.h"

/// /////////////////////////////////////////////////
/// エディタのコマンドを実行するシングルトンクラス
/// EditorManagerの関数を呼び出すだけ
/// /////////////////////////////////////////////////
class EditCommand final {
	friend class EditorManager;
private:
	EditCommand();
	~EditCommand();

	/// コピーコンストラクタと代入演算子を削除してシングルトンを保証
	EditCommand(const EditCommand&) = delete;
	EditCommand(EditCommand&&) = delete;
	EditCommand& operator=(const EditCommand&) = delete;
public:
	/// ==============================================
	/// public : methods
	/// ==============================================

	/// @brief コマンドの実行
	/// @tparam T 実行するコマンドの型
	/// @param ..._args コマンドの引数
	template <IsEditorCommand T, typename ...Args>
	static void Execute(Args&& ..._args);

	void Redo();
	void Undo();

private:
	/// ==============================================
	/// private : objects
	/// ==============================================

	/// EditorManagerのポインタ
	static EditorManager* pEditorManager_;
};


template<IsEditorCommand T, typename ...Args>
inline void EditCommand::Execute(Args&& ..._args) {
	pEditorManager_->ExecuteCommand<T>(_args);
}

