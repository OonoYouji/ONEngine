#pragma once

/// engine
#include "EditorManager.h"

/// /////////////////////////////////////////////////
/// エディタのコマンドを実行するシングルトンクラス
/// EditorManagerの関数を呼び出すだけ
/// /////////////////////////////////////////////////
namespace ONEngine {

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

	static void Redo();
	static void Undo();

	template <typename T>
	static void SetClipboardData(const T& _data) {
		pEditorManager_->clipboard_.Set(_data);
	}

	template <typename T>
	static T* GetClipboardData() {
		return pEditorManager_->clipboard_.Get<T>();
	}

private:
	/// ==============================================
	/// private : objects
	/// ==============================================

	/// EditorManagerのポインタ
	static EditorManager* pEditorManager_;
};


template<IsEditorCommand T, typename ...Args>
inline void EditCommand::Execute(Args&& ..._args) {
	pEditorManager_->ExecuteCommand<T>(_args...);
}


} /// ONEngine
