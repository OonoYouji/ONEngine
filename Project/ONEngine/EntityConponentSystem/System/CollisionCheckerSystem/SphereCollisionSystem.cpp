#include "SphereCollisionSystem.h"

#include "EntityConponentSystem/ECSManager/ECSManager.h"

#include "EntityConponentSystem/Component/PositionComponent/PositionComponent.h"
#include "EntityConponentSystem/Component/ColliderComponents/SphereColliderComponent.h"	

void SphereCollisionSystem::Update(size_t _entity, ECSManager* _ecsManager) {
	SphereColliderComponent* sphereCollider = _ecsManager->GetComponent<SphereColliderComponent>(_entity);
	PositionComponent*       position       = _ecsManager->GetComponent<PositionComponent>(_entity);

	/// componentを持っていたので処理に移る
	if(position && sphereCollider) {
		const std::vector<size_t>& entities = _ecsManager->GetEntities();

		/// 衝突相手がいなかったらやめる
		if(entities.empty()) {
			return;
		}
		
		for(auto& otherEntity : entities) {
			if(CheckCollision(otherEntity, _ecsManager, position, sphereCollider)) {

			}
		}
	}

}

bool SphereCollisionSystem::CheckCollision(
	size_t _otherEntity, ECSManager* _ecsManager, PositionComponent* _thisEntityPositon, SphereColliderComponent* _thisEntitySphereCollder) {

	SphereColliderComponent* sphereCollider = _ecsManager->GetComponent<SphereColliderComponent>(_otherEntity);
	PositionComponent*       position       = _ecsManager->GetComponent<PositionComponent>(_otherEntity);

	/// componentを持っていたので処理に移る
	if(position && sphereCollider) {
		Vec3  diff     = position->position - _thisEntityPositon->position;
		float distance = sphereCollider->radius + _thisEntitySphereCollder->radius;

		if(diff.Len() < distance) {
			return true;
		}
	}
	return false;
}

