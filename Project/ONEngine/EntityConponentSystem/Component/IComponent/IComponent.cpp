#include "IComponent.h"

#include "../../Entity/Entity.h"

void IComponent::SetOwner(Entity* _owner) {
	owner_ = _owner;
}
