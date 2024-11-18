#include "MoveSystem.h"


#include "EntityConponentSystem/Entity/Entity.h"

#include "EntityConponentSystem/Component/PositionComponent.h"
#include "EntityConponentSystem/Component/VelocityComponent.h"



void MoveSystem::Initialize() {

}

void MoveSystem::Update(Entity* _entity) {
	PositionComponent* position = _entity->GetComponent<PositionComponent>();
	VelocityComponent* velocity = _entity->GetComponent<VelocityComponent>();

	if(position && velocity) {
		position->position_ += velocity->velocity_;
	}
}
