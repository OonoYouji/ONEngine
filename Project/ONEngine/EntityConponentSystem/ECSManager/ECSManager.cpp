#include "ECSManager.h"


size_t ECSManager::nextCompArrayId_ = 0;

void ECSManager::Initialize() {
	nextEntityId_ = 0;
}

void ECSManager::Update() {

	for(auto& entity : entities_) {
		for(auto& system : systems_) {
			system->Update(entity, this);
		}
	}

}

size_t ECSManager::GenerateEntity() {
	size_t resultEntity = ++nextEntityId_;

	entities_.push_back(resultEntity);
	return resultEntity;
}

void ECSManager::RemoveEntity(size_t _entity) {

}

void ECSManager::AddSystem(ISystem* _system) {
	std::unique_ptr<ISystem> add(_system);
	systems_.push_back(std::move(add));
}



//
//void ECSManager::EnemyContainer::Add(size_t _entity) {
//	entities_.emplace_back();
//	if(entitiyToIndex_.size() < _entity) {
//		entitiyToIndex_.resize(_entity + 1);
//	} 
//	entitiyToIndex_[_entity] = entities_.size() - 1;
//}
//
//void ECSManager::EnemyContainer::Remove(const size_t _entity) {
//	if(entitiyToIndex_.size() < _entity) {
//		return;
//	}
//
//	size_t backIndex   = entities_.size() - 1;
//	size_t backEntity  = entities_.size();
//	size_t removeIndex = entitiyToIndex_[_entity];
//
//	/// 削除する要素が最後の要素でなければ
//	if(_entity != entities_.size()) {
//		entities_[removeIndex]     = backEntity;
//		entitiyToIndex_[backIndex] = removeIndex;
//	}
//
//	entities_.pop_back();
//}
