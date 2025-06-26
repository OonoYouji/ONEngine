#include "EntityCollection.h"

/// engine
#include "Engine/Core/DirectX12/Manager/DxManager.h"
#include "Engine/Script/MonoScriptEngine.h"
#include "Engine/ECS/Component/Components/ComputeComponents/Script/Script.h"

/// entity
#include "Engine/ECS/Entity/Entities/Camera/Camera.h"
#include "Engine/ECS/Entity/Entities/Camera/DebugCamera.h"

/// ecs
#include "Engine/ECS/EntityComponentSystem/EntityComponentSystem.h"

EntityCollection::EntityCollection(EntityComponentSystem* _ecs, DxManager* _dxManager)
	: pECS_(_ecs), pDxManager_(_dxManager) {

	pDxDevice_ = pDxManager_->GetDxDevice();

	factory_ = std::make_unique<EntityFactory>(pDxDevice_);
	entities_.reserve(256);
}

EntityCollection::~EntityCollection() {}

IEntity* EntityCollection::GenerateEntity(const std::string& _name, bool _isInit) {
	auto entity = factory_->Generate(_name);
	if (entity) {
		entities_.emplace_back(std::move(entity));

		/// 初期化
		IEntity* entityPtr = entities_.back().get();
		entityPtr->pEntityComponentSystem_ = pECS_;
		entityPtr->id_ = NewEntityID();
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
	camera->id_ = NewEntityID();
	camera->CommonInitialize();
	camera->Initialize();

	Camera* cameraPtr = camera.get();
	entities_.push_back(std::move(camera));
	cameras_.push_back(cameraPtr);

	return cameraPtr;
}

void EntityCollection::RemoveEntity(IEntity* _entity, bool _deleteChildren) {

	/// 破棄可能かチェック
	if (_entity == nullptr) {
		return;
	}

	auto doNotDestroyEntityItr = std::find_if(doNotDestroyEntities_.begin(), doNotDestroyEntities_.end(),
		[&_entity](IEntity* entity) {
			return entity == _entity;
		}
	);

	if (doNotDestroyEntityItr != doNotDestroyEntities_.end()) {
		Console::Log("Cannot remove entity: " + _entity->GetName() + " because it is marked as do not destroy.");
		return;
	}


	/// entityのidをチェック
	size_t id = _entity->id_;
	usedEntityIDs_.erase(std::remove(usedEntityIDs_.begin(), usedEntityIDs_.end(), id), usedEntityIDs_.end());
	removedEntityIDs_.push_back(id);

	/// 親子関係の解除
	_entity->RemoveParent();
	if (_deleteChildren) {
		for (auto& child : _entity->GetChildren()) {
			RemoveEntity(child, _deleteChildren); ///< 子供の親子関係を解除した後に再帰的に削除
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
	for (auto& entity : entities_) {
		RemoveEntity(entity.get(), true);
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

size_t EntityCollection::NewEntityID() {
	size_t resultId = 0;

	if (removedEntityIDs_.size() > 0) {
		resultId = removedEntityIDs_.front();
		removedEntityIDs_.pop_front();
		usedEntityIDs_.push_back(resultId);
	} else {
		resultId = usedEntityIDs_.size();
		usedEntityIDs_.push_back(resultId);
	}

	return resultId;
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

