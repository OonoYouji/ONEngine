#include "MovementSystem.h"

#include "EntityConponentSystem/Component/PositionComponent/PositionComponent.h"
#include "EntityConponentSystem/Component/VelocityComponent/VelocityComponent.h"

#include "EntityConponentSystem/ECSManager/ECSManager.h"


void MovementSystem::Update(size_t _entity, ECSManager* _ecsManager) {
	PositionComponent* position = _ecsManager->GetComponent<PositionComponent>(_entity);
	VelocityComponent* velocity = _ecsManager->GetComponent<VelocityComponent>(_entity);

	if(position && velocity) {
		position->position_ += velocity->velocity_;
	}
}
