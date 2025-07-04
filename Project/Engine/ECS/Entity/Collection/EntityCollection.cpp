#include "EntityCollection.h"

/// std
#include <fstream>

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"
#include "Engine/ECS/Entity/EntityJsonConverter.h"

/// entity
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Entity/Entities/Camera/DebugCamera.h"
#include "Engine/ECS/Entity/Entities/EmptyEntity/EmptyEntity.h"

/// ecs
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

EntityCollection::EntityCollection(EntityComponentSystem* _ecs, DxManager* _dxManager)
	: pECS_(_ecs), pDxManager_(_dxManager) {

	pDxDevice_ = pDxManager_->GetDxDevice();

	factory_ = std::make_unique<EntityFactory>(pDxDevice_);
	entities_.reserve(256);

	LoadPrefabAll();
}

EntityCollection::~EntityCollection() {}

IEntity* EntityCollection::GenerateEntity(const std::string& _name, bool _isInit, bool _isRuntime) {
	auto entity = factory_->Generate(_name);
	if (entity) {
		entities_.emplace_back(std::move(entity));

		/// 初期化
		IEntity* entityPtr = entities_.back().get();
		entityPtr->pEntityComponentSystem_ = pECS_;
		entityPtr->id_ = NewEntityID(_isRuntime);
		entityPtr->CommonInitialize();
		if (_isInit) {
			entityPtr->Initialize();
		}

		return entities_.back().get();
	}
	return nullptr;
}

Camera* EntityCollection::GenerateCamera() {
	std::unique_ptr<Camera> camera = std::make_unique<Camera>(pDxManager_->GetDxDevice());
	camera->pEntityComponentSystem_ = pECS_;
	camera->id_ = NewEntityID(false);
	camera->CommonInitialize();
	camera->Initialize();

	Camera* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	cameras_.push_back(cameraPtr);

	return cameraPtr;
}

void EntityCollection::RemoveEntity(IEntity* _entity, bool _deleteChildren) {

	if (_entity == nullptr) {
		return;
	}

	/// ------------------------------
	/// 破棄可能かチェック
	/// ------------------------------
	auto doNotDestroyEntityItr = std::find_if(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(),
		[&_entity](IEntity* entity) {
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

	/// entityのidをチェック
	int32_t id = _entity->id_;

	if (id > 0) {
		/// 初期化時のidから削除
		initEntityIDs_.usedIds.erase(std::remove(initEntityIDs_.usedIds.begin(), initEntityIDs_.usedIds.end(), id), initEntityIDs_.usedIds.end());
		initEntityIDs_.removedIds.push_back(id);
	} else if (id < 0) {
		/// 実行時のidから削除
		runtimeEntityIDs_.usedIds.erase(std::remove(runtimeEntityIDs_.usedIds.begin(), runtimeEntityIDs_.usedIds.end(), id), runtimeEntityIDs_.usedIds.end());
		runtimeEntityIDs_.removedIds.push_back(id);
	} else {
		Console::Log("Invalid entity ID: " + std::to_string(id));
		return;
	}

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
		[_entity](const std::unique_ptr<IEntity>& entity) {
			return entity.get() == _entity;
		}
	);

	if (entityItr != entities_.end()) {
		entities_.erase(entityItr, entities_.end());
	}


	/// カメラの削除
	auto cameraItr = std::remove_if(cameras_.begin(), cameras_.end(),
		[_entity](Camera* camera) {
			return camera == _entity;
		}
	);

	if (cameraItr != cameras_.end()) {
		/// mainのカメラなら nullptr に設定
		Camera* camera = *cameraItr;
		if (camera == mainCamera_) {
			mainCamera_ = nullptr;
		} else if (camera == mainCamera2D_) {
			mainCamera2D_ = nullptr;
		} else if (camera == debugCamera_) {
			debugCamera_ = nullptr;
		}
		cameras_.erase(cameraItr, cameras_.end());
	}

}

void EntityCollection::RemoveEntityAll() {
	std::vector<IEntity*> toRemove;
	toRemove.reserve(entities_.size()); // 最適化

	for (const auto& entity : entities_) {
		toRemove.push_back(entity.get()); // ポインタだけをコピー
	}

	for (auto& entity : toRemove) {
		if (!entity->GetParent()) {
			RemoveEntity(entity, true); ///< 全てのエンティティを削除する
		}
	}

}


void EntityCollection::UpdateEntities() {
	for (auto& entity : entities_) {
		if (entity->GetParent()) {
			continue;
		}

		UpdateEntity(entity.get());
	}
}

void EntityCollection::UpdateEntity(IEntity* _entity) {
	if (!_entity) {
		return;
	}

	if (!_entity->GetActive()) {
		return;
	}

	_entity->Update();
	_entity->UpdateTransform();

	for (auto& child : _entity->GetChildren()) {
		UpdateEntity(child);
	}
}

void EntityCollection::AddDoNotDestroyEntity(IEntity* _entity) {
	if (_entity == nullptr) {
		return;
	}

	/// 既に存在する場合は追加しない
	if (std::find(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(), _entity) != doNotDestroyEntities_.end()) {
		return;
	}

	doNotDestroyEntities_.push_back(_entity);
}

void EntityCollection::RemoveDoNotDestroyEntity(IEntity* _entity) {
	auto itr = std::remove(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(), _entity);
	if (itr != doNotDestroyEntities_.end()) {
		doNotDestroyEntities_.erase(itr);
	}
}

void EntityCollection::SetFactoryRegisterFunc(std::function<void(EntityFactory*)> _func) {
	factoryRegisterFunc_ = _func;
	if (factoryRegisterFunc_) {
		factoryRegisterFunc_(factory_.get());
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

void EntityCollection::LoadPrefabAll() {
	/// Assets/Prefabs フォルダから全てのプレハブを読み込む
	std::string prefabPath = "./Assets/Prefabs/";

	/// directoryがあるのかチェック
	if (!std::filesystem::exists(prefabPath)) {
		Console::Log("Prefab directory does not exist: " + prefabPath);
		return;
	}

	/// directoryを探索
	for (const auto& entry : std::filesystem::directory_iterator(prefabPath)) {

		/// JSONファイルのみを対象とする
		if (entry.is_regular_file() && entry.path().extension() == ".json") {

			/// pathをPrefabに渡して終了
			std::string prefabName = entry.path().stem().string();
			prefabs_[prefabName] = std::make_unique<EntityPrefab>(entry.path().string());
		}
	}


}

IEntity* EntityCollection::GenerateEntityFromPrefab(const std::string& _prefabName) {
	/// prefabが存在するかチェック
	auto prefabItr = prefabs_.find(_prefabName);
	if (prefabItr == prefabs_.end()) {
		Console::Log("Prefab not found: " + _prefabName);
		return 0;
	}

	/// prefabを取得
	EntityPrefab* prefab = prefabItr->second.get();

	/// entityを生成する
	EmptyEntity* entity = GenerateEntity<EmptyEntity>(true);
	if (entity) {
		entity->SetName(_prefabName + "(Clone)");
		entity->SetPrefabName(_prefabName);

		EntityJsonConverter::FromJson(prefab->GetJson(), entity);

		return entity;
	}

	return nullptr;
}



void EntityCollection::SetMainCamera(Camera* _camera) {
	mainCamera_ = _camera;
}

void EntityCollection::SetMainCamera(size_t _index) {
	mainCamera_ = cameras_[_index];
}

void EntityCollection::SetMainCamera2D(Camera* _camera) {
	mainCamera2D_ = _camera;
}

void EntityCollection::SetMainCamera2D(size_t _index) {
	mainCamera2D_ = cameras_[_index];
}

void EntityCollection::SetDebugCamera(Camera* _camera) {
	debugCamera_ = _camera;
}

void EntityCollection::SetDebugCamera(size_t _index) {
	debugCamera_ = cameras_[_index];
}

const std::vector<std::unique_ptr<IEntity>>& EntityCollection::GetEntities() const {
	return entities_;
}

const std::vector<Camera*>& EntityCollection::GetCameras() {
	return cameras_;
}

const Camera* EntityCollection::GetMainCamera() const {
	return mainCamera_;
}

Camera* EntityCollection::GetMainCamera() {
	return mainCamera_;
}

const Camera* EntityCollection::GetMainCamera2D() const {
	return mainCamera2D_;
}

Camera* EntityCollection::GetMainCamera2D() {
	return mainCamera2D_;
}

const Camera* EntityCollection::GetDebugCamera() const {
	return debugCamera_;
}

Camera* EntityCollection::GetDebugCamera() {
	return debugCamera_;
}

