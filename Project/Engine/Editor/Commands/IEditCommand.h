#pragma once

/// std
#include <memory>

namespace Editor {


/**
 * @enum EDITOR_STATE
 * @brief エディタのコマンド実行結果および現在の動作状態を表す列挙型
 */
enum EDITOR_STATE {
	 EDITOR_STATE_RUNNING, ///< コマンドが実行中（非同期処理等）であることを表す
	 EDITOR_STATE_FINISH,  ///< コマンドの処理が正常に完了したことを表す
	 EDITOR_STATE_FAILED,  ///< コマンドの処理が失敗したことを表す
};


/**
 * @class IEditCommand
 * @brief エディタ内での操作（オブジェクト移動、プロパティ変更など）の Undo / Redo をサポートするためのコマンドパターンインターフェースクラス
 */
class IEditCommand {
public:
	/// ==========================================
	/// public : methods
	/// ==========================================

	/**
	 * @brief コンストラクタ
	 */
	IEditCommand() = default;

	/**
	 * @brief デストラクタ
	 */
	virtual ~IEditCommand() = default;

	/**
	 * @brief コマンドの実行（あるいは Redo）処理を行います。
	 * @return コマンドの実行完了ステート
	 */
	virtual EDITOR_STATE Execute() = 0;

	/**
	 * @brief コマンドの取り消し（Undo）処理を行います。
	 * @return コマンドの取り消し完了ステート
	 */
	virtual EDITOR_STATE Undo() = 0;

};


} /// Editor
