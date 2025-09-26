#include "EntityCollection.h"

/// std
#include <fstream>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"

/// ecs
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

EntityCollection::EntityCollection(ECSGroup* _ecsGroup, DxManager* _dxManager)
	: pECSGroup_(_ecsGroup), pDxManager_(_dxManager) {

	pDxDevice_ = pDxManager_->GetDxDevice();
	entities_.reserve(256);

	LoadPrefabAll();
}

EntityCollection::~EntityCollection() {}

GameEntity* EntityCollection::GenerateEntity(bool _isRuntime) {
	auto entity = std::make_unique<GameEntity>();
	if (entity) {
		entities_.emplace_back(std::move(entity));

		/// 初期化
		GameEntity* entityPtr = entities_.back().get();
		entityPtr->pECSGroup_ = pECSGroup_;
		entityPtr->id_ = NewEntityID(_isRuntime);
		entityPtr->Awake();

		return entities_.back().get();
	}
	return nullptr;
}

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
		Console::Log("Invalid entity ID: " + std::to_string(_id));
		return;
	}
}

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

void EntityCollection::RemoveDoNotDestroyEntity(GameEntity* _entity) {
	auto itr = std::remove(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(), _entity);
	if (itr != doNotDestroyEntities_.end()) {
		doNotDestroyEntities_.erase(itr);
	}
}

int32_t EntityCollection::NewEntityID(bool _isRuntime) {
	int32_t resultId = 0;

	if (_isRuntime) {
		/* --- 実行時 --- */

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
		/* --- 初期化時 --- */
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

uint32_t EntityCollection::GetEntityId(const std::string& _name) {
	for (auto& entity : entities_) {
		if (entity->name_ == _name) {
			return static_cast<uint32_t>(entity->GetId());
		}
	}

	return 0;
}

GameEntity* EntityCollection::GetEntity(size_t _entityId) {
	/// idを検索
	auto itr = std::find_if(
		entities_.begin(), entities_.end(),
		[_entityId](const std::unique_ptr<GameEntity>& entity) {
			return entity->GetId() == _entityId;
		}
	);
	
	if (itr != entities_.end()) {
		return (*itr).get();
	}

	Console::LogWarning("Entity not found for ID: " + std::to_string(_entityId));
	return nullptr;
}

void EntityCollection::LoadPrefabAll() {
	/// Assets/Prefabs フォルダから全てのプレハブを読み込む
	std::string prefabPath = "./Assets/Prefabs/";

	std::vector<File> prefabFiles = Mathf::FindFiles(prefabPath, ".prefab");

	if (prefabFiles.empty()) {
		Console::Log("No prefab files found in: " + prefabPath);
		return;
	}

	/// directoryを探索
	for (const auto& file : prefabFiles) {
		prefabs_[file.second] = std::make_unique<EntityPrefab>(file.first);
	}
}

void EntityCollection::ReloadPrefab(const std::string& _prefabName) {
	auto itr = prefabs_.find(_prefabName);
	if (itr == prefabs_.end()) {
		/// もう一度Fileを探索して確認
		File file = Mathf::FindFile("./Assets/Prefabs/", _prefabName);

		if (file.first.empty()) {
			Console::LogWarning("Prefab not found: " + _prefabName);
			return;
		}

		///!< 複数あった場合は最初に見つかったものを使用する
		prefabs_[file.second] = std::make_unique<EntityPrefab>(file.first);

		itr = prefabs_.find(_prefabName);
	}

	/// prefabを再読み込み
	itr->second->Reload();
}

GameEntity* EntityCollection::GenerateEntityFromPrefab(const std::string& _prefabName, bool _isRuntime) {
	/// prefabが存在するかチェック
	auto prefabItr = prefabs_.find(_prefabName);
	if (prefabItr == prefabs_.end()) {
		Console::Log("Prefab not found: " + _prefabName);
		return 0;
	}

	/// prefabを取得
	EntityPrefab* prefab = prefabItr->second.get();

	/// entityを生成する
	GameEntity* entity = GenerateEntity(_isRuntime);
	if (entity) {
		const std::string name = Mathf::FileNameWithoutExtension(_prefabName);

		if (_isRuntime) {
			entity->SetName(name + "(Clone)");
		} else {
			entity->SetName(name);
		}
		entity->SetPrefabName(_prefabName);

		EntityJsonConverter::FromJson(prefab->GetJson(), entity);

		return entity;
	}

	return nullptr;
}

EntityPrefab* EntityCollection::GetPrefab(const std::string& _fileName) {
	if (prefabs_.contains(_fileName)) {
		return prefabs_[_fileName].get();
	}

	return nullptr;
}



void EntityCollection::SetMainCamera(CameraComponent* _camera) {
	mainCamera_ = _camera;
}

void EntityCollection::SetMainCamera2D(CameraComponent* _camera) {
	mainCamera2D_ = _camera;
}

CameraComponent* EntityCollection::GetMainCamera() {
	return mainCamera_;
}

CameraComponent* EntityCollection::GetMainCamera2D() {
	return mainCamera2D_;
}

const std::vector<std::unique_ptr<GameEntity>>& EntityCollection::GetEntities() const {
	return entities_;
}



