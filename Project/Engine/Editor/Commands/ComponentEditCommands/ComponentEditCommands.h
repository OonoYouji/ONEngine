#pragma once

/// std
#include <typeindex>
#include <string>
#include <unordered_map>

/// engine
#include "../IEditCommand.h"


namespace ONEngine {
/// 前方宣言
class GameEntity;
class ECSGroup;
class SceneManager;
class IComponent;
}



/// ///////////////////////////////////////////////
/// エンティティのデータ出力コマンド
/// ///////////////////////////////////////////////
namespace Editor {

/**
 * @class EntityDataOutputCommand
 * @brief エンティティのデータをJSONファイルとして外部出力（エクスポート）するコマンドクラス
 */
class EntityDataOutputCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity 出力対象となるエンティティへのポインタ
     */
	EntityDataOutputCommand(ONEngine::GameEntity* _entity);

    /**
     * @brief デストラクタ
     */
	~EntityDataOutputCommand() override = default;

	/**
	 * @brief コマンドの実行（エンティティデータをシリアライズしてファイル出力します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（エクスポート処理を取り消すため、出力したファイルを削除します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string outputFilePath_;
};

/**
 * @class EntityDataInputCommand
 * @brief 外部JSONファイルからデータを読み込み、エンティティのコンポーネント構成やパラメータを再構築（インポート）するコマンドクラス
 */
class EntityDataInputCommand : public IEditCommand {
public:
    /**
     * @brief デフォルトコンストラクタ
     */
	EntityDataInputCommand() = default;

    /**
     * @brief コンストラクタ
     * @param _entity 入力（ロード）を適用するエンティティへのポインタ
     */
	EntityDataInputCommand(ONEngine::GameEntity* _entity);

    /**
     * @brief デストラクタ
     */
	~EntityDataInputCommand() override = default;

	/**
	 * @brief コマンドの実行（JSONファイルからデシリアライズしてエンティティのパラメータを再設定します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（インポート実行前の状態へエンティティを書き戻します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

	/**
	 * @brief 対象となるエンティティを設定します。
	 */
	void SetEntity(ONEngine::GameEntity* _entity);

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string inputFilePath_;
};


/**
 * @class AddComponentCommand
 * @brief エンティティに対して動的に新しくコンポーネントを追加するコマンドクラス（Undo時にそのコンポーネントを削除します）
 */
class AddComponentCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity 追加対象となるエンティティへのポインタ
     * @param _componentName 追加するコンポーネントの型名（文字列）
     */
	AddComponentCommand(ONEngine::GameEntity* _entity, const std::string& _componentName);

    /**
     * @brief デストラクタ
     */
	~AddComponentCommand() override = default;

	/**
	 * @brief コマンドの実行（指定されたコンポーネントを生成・エンティティへ追加します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（追加したコンポーネントをエンティティから削除します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string componentName_;
};


/**
 * @class RemoveComponentCommand
 * @brief エンティティから指定されたコンポーネントを削除するコマンドクラス（Undo時には削除されたコンポーネントを同一IDで復元します）
 */
class RemoveComponentCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity 削除対象のコンポーネントを保持するエンティティ
     * @param _componentName 削除するコンポーネントの型名（文字列）
     * @param _resultItr 削除結果のイテレータを受け取るポインタ
     */
	RemoveComponentCommand(ONEngine::GameEntity* _entity, const std::string& _componentName, std::unordered_map<size_t, ONEngine::IComponent*>::iterator* _resultItr);

    /**
     * @brief デストラクタ
     */
	~RemoveComponentCommand() override = default;

	/**
	 * @brief コマンドの実行（エンティティから指定コンポーネントを切り離して削除リストへ移動します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（切り離したコンポーネントを元のエンティティへ差し戻し再構築します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	std::unordered_map<size_t, ONEngine::IComponent*>::iterator* pIterator_;
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string componentName_;
};


/**
 * @class ReloadAllScriptsCommand
 * @brief C# アセンブリのホットリロード時などに、ECSの動作を一度休止し、スクリプトデータおよびシステム定義をすべて再読み込みするエディタ専用コマンドクラス
 */
class ReloadAllScriptsCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _ecs 対象となるECSグループ
     * @param _sceneManager 現在のアセット/シーンマネージャー
     */
	ReloadAllScriptsCommand(ONEngine::ECSGroup* _ecs, ONEngine::SceneManager* _sceneManager);

    /**
     * @brief デストラクタ
     */
	~ReloadAllScriptsCommand() override = default;

	/**
	 * @brief コマンドの実行（C#スクリプトの再読み込みをトリガーし、ECSのバインドを再構築します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し（このコマンドは元に戻せません）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	/// ================================================
	/// private : objects
	/// ================================================
	ONEngine::ECSGroup* pEcsGroup_ = nullptr;
	ONEngine::SceneManager* pSceneManager_ = nullptr;
};


} /// Editor