#pragma once

/// std
#include <string>

/// externals
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Asset/Guid/Guid.h"

/// editor
#include "../IEditCommand.h"

namespace ONEngine {
class GameEntity;
class ECSGroup;
}


/// ///////////////////////////////////////////////////
/// ゲームオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////
namespace Editor {

/**
 * @class CreateGameObjectCommand
 * @brief エディタ上で空のゲームオブジェクト（GameEntity）を新規作成し、Undo時にそれを破壊・削除するコマンドクラス
 */
class CreateGameObjectCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _ecs 対象となるECSグループへのポインタ
     * @param _name 生成するエンティティのデフォルト名
     * @param _parentEntity 親となるオブジェクト（存在する場合）
     */
	CreateGameObjectCommand(ONEngine::ECSGroup* _ecs, const std::string& _name = "NewEntity", ONEngine::GameEntity* _parentEntity = nullptr);

    /**
     * @brief デストラクタ
     */
	~CreateGameObjectCommand();

	/**
	 * @brief コマンドの実行（オブジェクトを生成し、親子関係や名前を割り当てます）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（生成したオブジェクトをECSグループから破棄します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_ = nullptr;
	ONEngine::GameEntity* generatedEntity_ = nullptr;
	ONEngine::Guid generatedGuid_;
	ONEngine::Guid parentGuid_;
	const std::string entityName_;
};


/// ///////////////////////////////////////////////////
/// プリミティブなオブジェクトの作成コマンド
/// ///////////////////////////////////////////////////
/**
 * @class CreatePrimitiveCommand
 * @brief カメラ、ディレクショナルライト、デフォルトメッシュなど、特定のコンポーネントが最初からアタッチされたプリミティブオブジェクトを作成するコマンドクラス
 */
class CreatePrimitiveCommand : public IEditCommand {
public:
    /**
     * @enum Type
     * @brief 作成するプリミティブオブジェクトの種類
     */
	enum class Type {
		Camera,           ///< カメラコンポーネント付きのエンティティ
		DirectionalLight, ///< ディレクショナルライトコンポーネント付きのエンティティ
		Mesh,             ///< 標準的なメッシュレンダラー付きのエンティティ
	};

    /**
     * @brief コンストラクタ
     * @param _ecs 対象となるECSグループへのポインタ
     * @param _type 作成するプリミティブの種類
     * @param _parentEntity 親オブジェクト（任意）
     */
	CreatePrimitiveCommand(ONEngine::ECSGroup* _ecs, Type _type, ONEngine::GameEntity* _parentEntity = nullptr);

    /**
     * @brief デストラクタ
     */
	~CreatePrimitiveCommand() = default;

	/**
	 * @brief コマンドの実行（指定されたコンポーネントが付随したプリミティブエンティティを生成します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（生成したプリミティブオブジェクトをECSグループから破棄します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_ = nullptr;
	ONEngine::GameEntity* generatedEntity_ = nullptr;
	ONEngine::Guid generatedGuid_;
	ONEngine::Guid parentGuid_;
	Type type_;
};


/// ///////////////////////////////////////////////////
/// シーンに配置してあるオブジェクトの名前をへんこうする 
/// ///////////////////////////////////////////////////
/**
 * @class EntityRenameCommand
 * @brief エディタのヒエラルキーやインスペクター上でエンティティの名前を変更した際、Undo / Redo を可能にするコマンドクラス
 */
class EntityRenameCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity 名前を変更する対象のエンティティ
     * @param _newName 新しい名前文字列
     */
	EntityRenameCommand(ONEngine::GameEntity* _entity, const std::string& _newName);

    /**
     * @brief デストラクタ
     */
	~EntityRenameCommand() = default;

	/**
	 * @brief コマンドの実行（新しい名前を設定します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（元の古い名前に戻します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::GameEntity* pEntity_;
	std::string oldName_ = "";
	std::string newName_ = "";
};


/// ///////////////////////////////////////////////////
/// シーンにあるオブジェクトから新しいクラスを作る
/// ///////////////////////////////////////////////////
/**
 * @class CreateNewEntityClassCommand
 * @brief エディタで編集中のオブジェクト（GameEntity）のコンポーネント構成などをベースに、C#のカスタムエンティティ派生クラススクリプトファイルを新規作成するコマンドクラス
 */
class CreateNewEntityClassCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity 雛形となるエンティティへのポインタ
     * @param _outputFilePath 出力先となるファイルパス
     */
	CreateNewEntityClassCommand(ONEngine::GameEntity* _entity, const std::string& _outputFilePath);

    /**
     * @brief デストラクタ
     */
	~CreateNewEntityClassCommand() = default;

	/**
	 * @brief コマンドの実行（新規C#クラスファイルを生成します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（このコマンドは元に戻せません）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

	/**
	 * @brief テンプレートソースファイルをベースに、指定の名前空間やクラス名に置き換えて新しいスクリプトファイルを書き出します。
	 */
	EDITOR_STATE CreateNewClassFile(const std::string& _srcFilePath, const std::string& _outputFileName, const std::string& _newClassName);

private:
	ONEngine::GameEntity* pEntity_ = nullptr;

	std::string sourceClassPath_;
	std::string sourceClassName_;
	std::string outputFilePath_;
};


/// ///////////////////////////////////////////////////
/// プレハブを作成するコマンド
/// ///////////////////////////////////////////////////
/**
 * @class CreatePrefabCommand
 * @brief シーン上に存在するエンティティ階層をテンプレートアセットとして「プレハブ」ファイル（JSON形式）に保存するコマンドクラス
 */
class CreatePrefabCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity プレハブ化する対象の親エンティティ
     */
	CreatePrefabCommand(ONEngine::GameEntity* _entity);

    /**
     * @brief デストラクタ
     */
	~CreatePrefabCommand() = default;

	/**
	 * @brief コマンドの実行（プレハブファイルを出力します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（出力されたプレハブファイルを削除します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

	/**
	 * @brief 再帰的にエンティティ階層および各コンポーネントのシリアライズ情報を構築します。
	 * @param _entity 現在シリアライズするエンティティ
	 * @param _json 出力先JSONオブジェクト
	 */
	void SerializeRecursive(ONEngine::GameEntity* _entity, nlohmann::json& _json);

private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	std::string prefabPath_ = "./Assets/Prefabs/";
	std::string prefabName_ = "NewPrefab.json";
};


/// ///////////////////////////////////////////////////
/// エンティティを削除するコマンド
/// ///////////////////////////////////////////////////
/**
 * @class DeleteEntityCommand
 * @brief エディタのシーン・ヒエラルキー上から指定のエンティティを破壊・削除するコマンドクラス（Undo時にはすべてのコンポーネント・親子関係を含めて復元します）
 */
class DeleteEntityCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _ecs 対象となるECSグループ
     * @param _entity 削除対象のエンティティポインタ
     */
	DeleteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _entity);

    /**
     * @brief デストラクタ
     */
	~DeleteEntityCommand() = default;

	/**
	 * @brief コマンドの実行（エンティティをECSグループから破棄します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（削除されたエンティティ階層とコンポーネントを完全に再構築します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_;
	ONEngine::GameEntity* pEntity_;
};


/// ///////////////////////////////////////////////////
/// エンティティをコピーするコマンド
/// ///////////////////////////////////////////////////
/**
 * @class CopyEntityCommand
 * @brief 選択されたエンティティの全パラメータをクリップボード用一時バッファ（JSON）に一時保存するコマンドクラス
 */
class CopyEntityCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity コピー元のエンティティポインタ
     */
	CopyEntityCommand(ONEngine::GameEntity* _entity);

    /**
     * @brief デストラクタ
     */
	~CopyEntityCommand() = default;

	/**
	 * @brief コマンドの実行（クリップボード用のシリアライズ処理を行います）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（コピー状態の取り消しは不要なため何もしません）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;
private:
	ONEngine::GameEntity* pEntity_;
	nlohmann::json entityJson_;
};


/// ///////////////////////////////////////////////////
/// エンティティをペーストするコマンド
/// ///////////////////////////////////////////////////
/**
 * @class PasteEntityCommand
 * @brief クリップボードバッファにあるJSON情報からエンティティを複製ペーストするコマンドクラス
 */
class PasteEntityCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _ecs 対象となるECSグループ
     * @param _selectedEntity 現在選択されているエンティティ（ペースト先の親指定など）
     */
	PasteEntityCommand(ONEngine::ECSGroup* _ecs, ONEngine::GameEntity* _selectedEntity);

    /**
     * @brief デストラクタ
     */
	~PasteEntityCommand() = default;

	/**
	 * @brief コマンドの実行（JSONをデシリアライズして新規エンティティとしてインスタンス化します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（ペーストしたオブジェクトをシーンから破棄します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_;
	ONEngine::GameEntity* pSelectedEntity_ = nullptr;
	ONEngine::GameEntity* pastedEntity_ = nullptr;
};

/// ///////////////////////////////////////////////////
/// エンティティの親子付けを変更するコマンド
/// ///////////////////////////////////////////////////
/**
 * @class ChangeEntityParentCommand
 * @brief ドラッグ＆ドロップなどでヒエラルキー階層での親オブジェクトを付け替えるコマンドクラス（Undoで元の親階層に戻します）
 */
class ChangeEntityParentCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _entity 親子付けを変更する対象のオブジェクト
     * @param _newParent 新しい親オブジェクトへのポインタ
     */
	ChangeEntityParentCommand(ONEngine::GameEntity* _entity, ONEngine::GameEntity* _newParent);

    /**
     * @brief デストラクタ
     */
	~ChangeEntityParentCommand() = default;

	/**
	 * @brief コマンドの実行（親ポインタを変更し、相対位置などを再計算して同期します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（元の古い親オブジェクトへ戻します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;
private:
	ONEngine::GameEntity* pEntity_ = nullptr;
	ONEngine::GameEntity* pNewParent_ = nullptr;
	ONEngine::GameEntity* pOldParent_ = nullptr;
};

/// ///////////////////////////////////////////////////
/// エンティティの順番を入れ替えるコマンド
/// ///////////////////////////////////////////////////
/**
 * @class ReorderEntityCommand
 * @brief ヒエラルキーのリスト内における表示順序を入れ替えるコマンドクラス
 */
class ReorderEntityCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _ecsGroup 対象となるECSグループ
     * @param _entity 並び順を変更するエンティティ
     * @param _newParent 並び替え後の親オブジェクト
     * @param _newIndex 新しいインデックス値
     */
	ReorderEntityCommand(ONEngine::ECSGroup* _ecsGroup, ONEngine::GameEntity* _entity, ONEngine::GameEntity* _newParent, uint32_t _newIndex);

    /**
     * @brief デストラクタ
     */
	~ReorderEntityCommand() = default;

	/**
	 * @brief コマンドの実行（指定のインデックス位置にエンティティを並び替えます）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（並び替え前のインデックスに戻します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_;
	ONEngine::GameEntity* pEntity_;
	ONEngine::GameEntity* pNewParent_;
	ONEngine::GameEntity* pOldParent_;
	uint32_t newIndex_;
	uint32_t oldIndex_;
};

/// ///////////////////////////////////////////////////
/// プレハブからインスタンスを作成するコマンド
/// ///////////////////////////////////////////////////
/**
 * @class InstantiatePrefabCommand
 * @brief 保存されたプレハブファイル（JSON形式）をロードし、シーン上にインスタンス化するコマンドクラス
 */
class InstantiatePrefabCommand : public IEditCommand {
public:
    /**
     * @brief コンストラクタ
     * @param _ecs 対象となるECSグループ
     * @param _prefabPath プレハブファイルのパス
     * @param _parentEntity 親となるエンティティ（任意）
     */
	InstantiatePrefabCommand(ONEngine::ECSGroup* _ecs, const std::string& _prefabPath, ONEngine::GameEntity* _parentEntity = nullptr);

    /**
     * @brief デストラクタ
     */
	~InstantiatePrefabCommand() = default;

	/**
	 * @brief コマンドの実行（プレハブ情報をデシリアライズして生成します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Execute() override;

	/**
	 * @brief コマンドの取り消し処理（生成されたオブジェクト階層を削除します）。
	 * @return 実行結果ステート
	 */
	EDITOR_STATE Undo() override;

private:
	ONEngine::ECSGroup* pEcsGroup_ = nullptr;
	ONEngine::GameEntity* generatedEntity_ = nullptr;
	ONEngine::Guid generatedGuid_;
	ONEngine::Guid parentGuid_;
	const std::string prefabPath_;
};

} /// Editor
