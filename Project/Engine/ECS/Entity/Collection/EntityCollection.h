#pragma once

/// std
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <deque>

/// externals
#include <nlohmann/json_fwd.hpp>

#include "../GameEntity/GameEntity.h"
#include "../Prefab/EntityPrefab.h"

namespace ONEngine {

class CameraComponent;

/// ///////////////////////////////////////////////////
/// Entityのコレクションクラス
/// ///////////////////////////////////////////////////
/**
 * @class EntityCollection
 * @brief シーン上に存在するすべてのゲームエンティティの生成、ID・GUID管理、親子ツリー操作、シーン遷移時非破棄、プレハブ生成・反映などを一元管理するコンテナクラス
 */
class EntityCollection final {
private:
	/// =========================================
	/// private : sub classes
	/// =========================================

	/// @brief EntityIdの管理用コンテナ
	struct IdContainer {
		std::deque<int32_t> usedIds;    ///< 使用中のID
		std::deque<int32_t> removedIds; ///< 削除されたID
	};

public:
	/// =========================================
	/// public : methods
	/// =========================================

	/**
	 * @brief コンストラクタ
	 * @param _ecsGroup 所属するECSグループ
	 * @param _dxm DX12システムマネージャ
	 */
	EntityCollection(class ECSGroup* _ecsGroup, class DxManager* _dxm);
	
	/**
	 * @brief デストラクタ
	 */
	~EntityCollection();

	/**
	 * @brief 指定されたGUIDを使用して、新規にゲームエンティティを生成して管理下に置きます。
	 * @param _guid ユニークなGUID
	 * @param _isRuntime 実行時の一時オブジェクトかどうか
	 * @return 生成されたGameEntityポインタ
	 */
	GameEntity* GenerateEntity(const Guid& _guid, bool _isRuntime = false);
	
	/**
	 * @brief 新規のエンティティに割り当てるためのIDを発行します。
	 * @param _isRuntime 実行時の一時オブジェクト用ID（負の値）を発行するかどうか
	 * @return 新規発行されたID
	 */
	int32_t NewEntityID(bool _isRuntime);

	/**
	 * @brief 指定した名前のエンティティIDを取得します。
	 * @param _name 検索するエンティティ名
	 * @return ID（見つからない場合はエラー値）
	 */
	uint32_t GetEntityId(const std::string& _name);
	
	/**
	 * @brief 配列内インデックスあるいはIDから特定のゲームエンティティを取得します。
	 * @param _entityId インデックスID
	 * @return GameEntityポインタ。見つからない場合は nullptr
	 */
	GameEntity* GetEntity(int32_t _entityId);
	
	/**
	 * @brief GUIDをもとに特定のゲームエンティティを検索して取得します。
	 * @param _guid 検索対象のGUID
	 * @return GameEntityポインタ。存在しない場合は nullptr
	 */
	GameEntity* GetEntityFromGuid(const Guid& _guid);

	/**
	 * @brief 指定されたエンティティを破棄します（子エンティティも同時に破棄可能です）。
	 * @param _entity 破棄対象エンティティ
	 * @param _deleteChildren trueの場合、子エンティティもツリーから削除・破棄します
	 */
	void RemoveEntity(GameEntity* _entity, bool _deleteChildren = true);
	
	/**
	 * @brief 使用済みIDを管理コンテナから削除・再利用キューに返却します。
	 * @param _id 返却するID
	 */
	void RemoveEntityId(int32_t _id);
	
	/**
	 * @brief 管理しているすべてのエンティティを削除（クリーンアップ）します。
	 */
	void RemoveEntityAll();

	/**
	 * @brief シーン遷移時に破棄しない非破棄（DontDestroyOnLoad）エンティティを追加します。
	 * @param _entity 対象エンティティ
	 */
	void AddDoNotDestroyEntity(GameEntity* _entity);
	
	/**
	 * @brief 非破棄エンティティリストから除外します。
	 * @param _entity 対象エンティティ
	 */
	void RemoveDoNotDestroyEntity(GameEntity* _entity);

	/**
	 * @brief エンティティのリスト内順序（ヒエラルキーのソート等）を入れ替えます。
	 * @param _entity 移動対象エンティティ
	 * @param _newIndex 移動先のインデックス
	 */
	void MoveEntity(GameEntity* _entity, size_t _newIndex);




	/* ----- prefab ----- */

	/**
	 * @brief Prefabsフォルダ配下の全プレハブアセットファイルを読み込みます。
	 */
	void LoadPrefabAll();
	
	/**
	 * @brief 指定したプレハブをファイルから再ロードし、キャッシュを更新します。
	 * @param _prefabName 再ロードするプレハブアセット名
	 */
	void ReloadPrefab(const std::string& _prefabName);

	/**
	 * @brief 指定されたプレハブ情報に基づいて、新規エンティティをクローン生成します。
	 * @param _prefabName 元となるプレハブ名
	 * @param _isRuntime 実行時の一時オブジェクトとするか
	 * @return 生成されたGameEntityポインタ
	 */
	GameEntity* GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime = true);
	
	/**
	 * @brief 指定したファイル名のプレハブオブジェクトを取得します。
	 * @param _fileName プレハブアセットファイル名
	 * @return EntityPrefabポインタ
	 */
	EntityPrefab* GetPrefab(const std::string& _fileName);

	/**
	 * @brief 既存のエンティティに対してプレハブの構成データ（コンポーネント等）を同期・適用します。
	 * @param _entity 反映先エンティティ
	 * @param _prefabName 元となるプレハブ名
	 */
	void ApplyPrefabToEntity(GameEntity* _entity, const std::string& _prefabName);

private:


	/**
	 * @brief シリアライズされたJSONデータをもとに、親子関係を含め再帰的にエンティティおよびコンポーネントを復元生成します。
	 * @param _json シリアライズデータ
	 * @param _entity 親エンティティ（ルート復元時は nullptr）
	 * @param _isRuntime 実行時の一時オブジェクトとして生成するか
	 * @return 復元されたGameEntityポインタ
	 */
	GameEntity* GenerateEntityRecursive(const nlohmann::json& _json, GameEntity* _entity, bool _isRuntime);


private:
	/// =========================================
	/// private : objects
	/// =========================================

	class ECSGroup* pEcsGroup_;
	class DxManager* pDxManager_;
	class DxDevice* pDxDevice_;

	/// entityのIDを管理するためのdeque
	IdContainer initEntityIDs_;
	IdContainer runtimeEntityIDs_;

	/// entityの本体を持つ配列
	std::vector<std::unique_ptr<GameEntity>> entities_;
	std::vector<GameEntity*> doNotDestroyEntities_;
	std::unordered_map<Guid, GameEntity*> guidEntityMap_;

	CameraComponent* mainCamera_ = nullptr;
	CameraComponent* mainCamera2D_ = nullptr;

	/// prefab
	std::unordered_map<std::string, std::unique_ptr<EntityPrefab>> prefabs_;

public:
	/// =========================================
	/// public : accessor
	/// =========================================

	/**
	 * @brief メイン3Dカメラを設定します。
	 */
	void SetMainCamera(CameraComponent* _cameraComponent);
	
	/**
	 * @brief メイン2Dカメラを設定します。
	 */
	void SetMainCamera2D(CameraComponent* _cameraComponent);

	/**
	 * @brief メイン3Dカメラオブジェクトを取得します。
	 */
	CameraComponent* GetMainCamera();
	
	/**
	 * @brief メイン2Dカメラオブジェクトを取得します。
	 */
	CameraComponent* GetMainCamera2D();

	/**
	 * @brief コレクション内の全ゲームエンティティのリスト（読み取り専用）を取得します。
	 */
	const std::vector<std::unique_ptr<GameEntity>>& GetEntities() const;

};

} /// ONEngine
