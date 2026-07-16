#include "EntityCollection.h"
#include <nlohmann/json.hpp>

using namespace ONEngine;

/// std
#include <fstream>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"

/// ecs
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

EntityCollection::EntityCollection(ECSGroup* _ecsGroup, DxManager* _dxm)
	: pEcsGroup_(_ecsGroup), pDxManager_(_dxm) {
	mainCamera_ = nullptr;
	mainCamera2D_ = nullptr;
	pDxDevice_ = pDxManager_->GetDxDevice();
	entities_.reserve(256);

	LoadPrefabAll();
}

EntityCollection::~EntityCollection() {}

/**
 * @brief 指定されたGUIDを使用して、新規にゲームエンティティを生成して管理下に置きます。
 */
GameEntity* EntityCollection::GenerateEntity(const Guid& _guid, bool _isRuntime) {
	Console::LogInfo("[SOURCE_DETECTOR] Engine creating Entity (Internal)");
	auto entity = std::make_unique<GameEntity>();
	if (entity) {
		entities_.emplace_back(std::move(entity));

		/// 初期化
		GameEntity* entityPtr = entities_.back().get();
		entityPtr->pEcsGroup_ = pEcsGroup_;
		entityPtr->id_ = NewEntityID(_isRuntime);
		entityPtr->guid_ = _guid;
		entityPtr->Awake();

		guidEntityMap_[_guid] = entityPtr;

		return entities_.back().get();
	}
	return nullptr;
}

/**
 * @brief 指定されたエンティティを破棄します（子エンティティも同時に破棄可能です）。
 */
void EntityCollection::RemoveEntity(GameEntity* _entity, bool _deleteChildren) {

	if (_entity == nullptr) {
		return;
	}

	/// ------------------------------
	/// 破棄可能かチェック
	/// ------------------------------
	auto doNotDestroyEntityItr = std::find_if(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(),
		[&_entity](GameEntity* entity) {
			return entity == _entity;
		}
	);

	if (doNotDestroyEntityItr != doNotDestroyEntities_.end()) {
		Console::Log("Cannot remove entity: " + _entity->GetName() + " because it is marked as do not destroy.");
		return;
	}


	/// ------------------------------
	/// 実際に破棄する
	/// ------------------------------

	/// Componentの破棄
	_entity->RemoveComponentAll();

	guidEntityMap_.erase(_entity->GetGuid());
	RemoveEntityId(_entity->GetId());

	/// 親子関係の解除
	_entity->RemoveParent();
	if (_deleteChildren) {

		if (_entity->GetChildren().size() > 0) {
			auto children = _entity->GetChildren();
			for (auto& child : children) {
				RemoveEntity(child, _deleteChildren); ///< 子供の親子関係を解除した後に再帰的に削除
			}
		}
	} else {
		for (auto& child : _entity->GetChildren()) {
			child->RemoveParent();
		}
	}

	/// entityの削除
	auto entityItr = std::remove_if(entities_.begin(), entities_.end(),
		[_entity](const std::unique_ptr<GameEntity>& entity) {
			return entity.get() == _entity;
		}
	);

	if (entityItr != entities_.end()) {
		entities_.erase(entityItr, entities_.end());
	}

}

/**
 * @brief 使用済みIDを管理コンテナから削除・再利用キューに返却します。
 */
void EntityCollection::RemoveEntityId(int32_t _id) {
	if (_id > 0) {
		/// 初期化時のidから削除
		initEntityIDs_.usedIds.erase(std::remove(initEntityIDs_.usedIds.begin(), initEntityIDs_.usedIds.end(), _id), initEntityIDs_.usedIds.end());
		initEntityIDs_.removedIds.push_back(_id);
	} else if (_id < 0) {
		/// 実行時のidから削除
		runtimeEntityIDs_.usedIds.erase(std::remove(runtimeEntityIDs_.usedIds.begin(), runtimeEntityIDs_.usedIds.end(), _id), runtimeEntityIDs_.usedIds.end());
		runtimeEntityIDs_.removedIds.push_back(_id);
	} else {
		Console::LogWarning("Invalid entity ID: " + std::to_string(_id));
		return;
	}
}

/**
 * @brief 管理しているすべてのエンティティを削除（クリーンアップ）します。
 */
void EntityCollection::RemoveEntityAll() {
	std::vector<GameEntity*> toRemove;
	toRemove.reserve(entities_.size()); // 最適化

	for (const auto& entity : entities_) {
		/// 親がいるエンティティは再帰的に処理されるのでここでは追加しない
		if (!entity->GetParent()) {
			toRemove.push_back(entity.get()); // ポインタだけをコピー
		}
	}

	for (auto& entity : toRemove) {
		if (!entity->GetParent()) {
			RemoveEntity(entity, true); ///< 全てのエンティティを削除する
		}
	}

}

/**
 * @brief シーン遷移時に破棄しない非破棄（DontDestroyOnLoad）エンティティを追加します。
 */
void EntityCollection::AddDoNotDestroyEntity(GameEntity* _entity) {
	if (_entity == nullptr) {
		return;
	}

	/// 既に存在する場合は追加しない
	if (std::find(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(), _entity) != doNotDestroyEntities_.end()) {
		return;
	}

	doNotDestroyEntities_.push_back(_entity);
}

/**
 * @brief 非破棄エンティティリストから除外します。
 */
void EntityCollection::RemoveDoNotDestroyEntity(GameEntity* _entity) {
	auto itr = std::remove(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(), _entity);
	if (itr != doNotDestroyEntities_.end()) {
		doNotDestroyEntities_.erase(itr);
	}
}

/**
 * @brief エンティティのリスト内順序（ヒエラルキーのソート等）を入れ替えます。
 */
void EntityCollection::MoveEntity(GameEntity* _entity, size_t _newIndex) {
	auto it = std::find_if(entities_.begin(), entities_.end(), [_entity](const std::unique_ptr<GameEntity>& e) {
		return e.get() == _entity;
	});

	if (it != entities_.end()) {
		std::unique_ptr<GameEntity> entityPtr = std::move(*it);
		entities_.erase(it);

		if (_newIndex > entities_.size()) {
			_newIndex = entities_.size();
		}
		entities_.insert(entities_.begin() + _newIndex, std::move(entityPtr));
	}
}

/**
 * @brief 新規のエンティティに割り当てるためのIDを発行します。
 */
int32_t EntityCollection::NewEntityID(bool _isRuntime) {
	int32_t resultId = 0;

	if (_isRuntime) {
		/// Runtime起動後のID生成

		// 削除されたIDがあればそれを使用
		if (runtimeEntityIDs_.removedIds.size() > 0) {
			resultId = runtimeEntityIDs_.removedIds.front();
			runtimeEntityIDs_.removedIds.pop_front();
			runtimeEntityIDs_.usedIds.push_back(resultId);
		} else {
			// なければ新しいIDを生成
			resultId = static_cast<int32_t>(runtimeEntityIDs_.usedIds.size() + 1) * -1;
			runtimeEntityIDs_.usedIds.push_back(resultId);
		}


	} else {

		/// Runtime起動前のID生成
		if (initEntityIDs_.removedIds.size() > 0) {
			resultId = initEntityIDs_.removedIds.front();
			initEntityIDs_.removedIds.pop_front();
			initEntityIDs_.usedIds.push_back(resultId);
		} else {
			resultId = static_cast<int32_t>(initEntityIDs_.usedIds.size()) + 1;
			initEntityIDs_.usedIds.push_back(resultId);
		}
	}

	return resultId;
}

/**
 * @brief 指定した名前のエンティティIDを取得します。
 */
uint32_t EntityCollection::GetEntityId(const std::string& _name) {
	for (auto& entity : entities_) {
		if (entity->name_ == _name) {
			return static_cast<uint32_t>(entity->GetId());
		}
	}

	return 0;
}

/**
 * @brief 配列内インデックスあるいはIDから特定のゲームエンティティを取得します。
 */
GameEntity* EntityCollection::GetEntity(int32_t _entityId) {
	auto itr = std::find_if(entities_.begin(), entities_.end(),
		[_entityId](const std::unique_ptr<GameEntity>& entity) {
			return entity->GetId() == _entityId;
		}
	);

	if (itr != entities_.end()) {
		return (*itr).get();
	}

	return nullptr;
}

/**
 * @brief GUIDをもとに特定のゲームエンティティを検索して取得します。
 */
GameEntity* EntityCollection::GetEntityFromGuid(const Guid& _guid) {
	auto itr = guidEntityMap_.find(_guid);
	if (itr != guidEntityMap_.end()) {
		return itr->second;
	}
	Console::LogWarning("Entity not found for Guid: " + _guid.ToString());
	return nullptr;
}

/**
 * @brief Prefabsフォルダ配下の全プレハブアセットファイルを読み込みます。
 */
void EntityCollection::LoadPrefabAll() {
	/// Assets/Prefabs フォルダから全てのプレハブを読み込む
	std::string prefabPath = "./Assets/Prefabs/";

	std::vector<File> prefabFiles = FileSystem::GetFiles(prefabPath, ".prefab");

	if (prefabFiles.empty()) {
		Console::Log("No prefab files found in: " + prefabPath);
		return;
	}

	/// directoryを探索
	for (const auto& file : prefabFiles) {
		prefabs_[file.second] = std::make_unique<EntityPrefab>(file.first);
	}
}

/**
 * @brief 指定したプレハブをファイルから再ロードし、キャッシュを更新します。
 */
void EntityCollection::ReloadPrefab(const std::string& _prefabName) {
	auto itr = prefabs_.find(_prefabName);
	if (itr == prefabs_.end()) {
		// 拡張子なしで検索された場合に備えて ".prefab" を付けて再試行
		std::string nameWithExt = _prefabName + ".prefab";
		itr = prefabs_.find(nameWithExt);
	}

	if (itr == prefabs_.end()) {
		/// もう一度Fileを探索して確認
		File file = FileSystem::GetFile("./Assets/Prefabs/", _prefabName);

		if (file.first.empty()) {
			Console::LogWarning("Prefab not found: " + _prefabName);
			return;
		}

		///!< 複数あった場合は最初に見つかったものを使用する
		prefabs_[file.second] = std::make_unique<EntityPrefab>(file.first);

		itr = prefabs_.find(file.second);
	}

	// 1. 各エンティティの現在の「カスタム変更（差分）」を一時保存する
	// ※リロード前のPrefab状態と比較して、何がオーバーライドされているかを記録
	const std::string& actualPrefabName = itr->first;
	std::unordered_map<GameEntity*, nlohmann::json> entityDiffs;
	for (auto& entity : entities_) {
		if (entity->GetPrefabName() == actualPrefabName || 
			(!actualPrefabName.empty() && entity->GetPrefabName() == FileSystem::FileNameWithoutExtension(actualPrefabName))) {
			// ToJson(entity, false) は現在の Prefab との差分を返す
			entityDiffs[entity.get()] = EntityJsonConverter::ToJson(entity.get(), false);
		}
	}

	/// 2. prefabをディスクから再読み込み（これでベースラインが更新される）
	itr->second->Reload();

	/// 3. 更新されたPrefabベースに、保存しておいた差分を再適用する
	for (auto& [entity, diff] : entityDiffs) {
		// まず、新しいPrefabを強制的に適用（ベースラインの更新）
		ApplyPrefabToEntity(entity, actualPrefabName);
		
		// 次に、保存しておいたカスタム差分をマージ適用（オーバーライドの復元）
		EntityJsonConverter::FromJson(diff, entity, pEcsGroup_->GetGroupName(), true);
	}
}

/**
 * @brief 指定されたプレハブ情報に基づいて、新規エンティティをクローン生成します。
 */
GameEntity* EntityCollection::GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime) {
	/// prefabが存在するかチェック
	auto prefabItr = prefabs_.find(_prefabName);
	if (prefabItr == prefabs_.end()) {
		// 拡張子なしで検索された場合に備えて ".prefab" を付けて再試行
		std::string nameWithExt = _prefabName + ".prefab";
		prefabItr = prefabs_.find(nameWithExt);

		if (prefabItr == prefabs_.end()) {
			Console::LogError("Prefab not found: " + _prefabName);
			return nullptr;
		}
	}

	/// prefabを取得
	EntityPrefab* prefab = prefabItr->second.get();

	/// entityを生成する
	return GenerateEntityRecursive(prefab->GetJson(), nullptr, _isRuntime);
}

/**
 * @brief 指定したファイル名のプレハブオブジェクトを取得します。
 */
EntityPrefab* EntityCollection::GetPrefab(const std::string& _fileName) {
	if (prefabs_.contains(_fileName)) {
		return prefabs_[_fileName].get();
	}

	// 拡張子なしで検索された場合に備えて ".prefab" を付けて再試行
	std::string nameWithExt = _fileName + ".prefab";
	if (prefabs_.contains(nameWithExt)) {
		return prefabs_[nameWithExt].get();
	}

	return nullptr;
}

/**
 * @brief 既存のエンティティに対してプレハブの構成データ（コンポーネント等）を同期・適用します。
 */
void EntityCollection::ApplyPrefabToEntity(GameEntity* _entity, const std::string& _prefabName) {
	if (!_entity) {
		Console::LogError("Entity is null when applying prefab: " + _prefabName);
		return;
	}

	/// prefabが存在するかチェック
	auto prefabItr = prefabs_.find(_prefabName);
	if (prefabItr == prefabs_.end()) {
		// 拡張子なしで検索された場合に備えて ".prefab" を付けて再試行
		std::string nameWithExt = _prefabName + ".prefab";
		prefabItr = prefabs_.find(nameWithExt);

		if (prefabItr == prefabs_.end()) {
			Console::LogError("Prefab not found: " + _prefabName);
			return;
		}
	}

	/// prefabを取得
	EntityPrefab* prefab = prefabItr->second.get();

	/// Jsonからコンポーネントを生成 (Prefab適用時はマージではなく上書き)
	EntityJsonConverter::FromJson(prefab->GetJson(), _entity, pEcsGroup_->GetGroupName(), false);
	}

/**
 * @brief シリアライズされたJSONデータをもとに、親子関係を含め再帰的にエンティティおよびコンポーネントを復元生成します。
 */
GameEntity* EntityCollection::GenerateEntityRecursive(const nlohmann::json& _json, GameEntity* _parent, bool _isRuntime) {

	/*
	* Guidはファイル内のものではなく新規生成する
	* でないと同じPrefabから生成したときにGuidが被ってしまう
	*/

	/// entityを生成する
	GameEntity* entity = GenerateEntity(GenerateGuid(), _isRuntime);
	if (!entity) {
		Console::LogError("Failed to generate entity from JSON.");
		return nullptr;
	}

	/// 親子関係の設定
	if (_parent) {
		entity->SetParent(_parent);
	}

	/// 名前とPrefab名を設定
	const std::string prefabName = _json.value("prefabName", "");
	std::string name = FileSystem::FileNameWithoutExtension(prefabName);
	if (prefabName.empty()) {
		name = _json.value("name", "New Entity");
	}

	/// runtime中かどうかで名前を変更
	if (_isRuntime) {
		entity->SetName(name + "(Clone)");
	} else {
		entity->SetName(name);
	}

	entity->SetPrefabName(prefabName);

	/// Jsonからコンポーネントを生成
	EntityJsonConverter::FromJson(_json, entity, pEcsGroup_->GetGroupName());

	/// 子エンティティの生成
	if (_json.contains("children")) {
		for (auto& childJson : _json["children"]) {
			GenerateEntityRecursive(childJson, entity, _isRuntime);
		}
	}

	return entity;
}



/**
 * @brief メイン3Dカメラを設定します。
 */
void EntityCollection::SetMainCamera(CameraComponent* _camera) {
	mainCamera_ = _camera;
}

/**
 * @brief メイン2Dカメラを設定します。
 */
void EntityCollection::SetMainCamera2D(CameraComponent* _camera) {
	mainCamera2D_ = _camera;
}

/**
 * @brief メイン3Dカメラオブジェクトを取得します。
 */
CameraComponent* EntityCollection::GetMainCamera() {
	return mainCamera_;
}

/**
 * @brief メイン2Dカメラオブジェクトを取得します。
 */
CameraComponent* EntityCollection::GetMainCamera2D() {
	return mainCamera2D_;
}

/**
 * @brief コレクション内の全ゲームエンティティのリスト（読み取り専用）を取得します。
 */
const std::vector<std::unique_ptr<GameEntity>>& EntityCollection::GetEntities() const {
	return entities_;
}



