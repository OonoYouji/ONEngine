#include "DemoEntity.h"

#include "../Component/PositionComponent.h"
#include "../Component/VelocityComponent.h"

DemoEntity::DemoEntity() : Entity(0) {
	AddComponent<PositionComponent>();
	AddComponent<VelocityComponent>();
}
