#include "IComponent.h"

void IComponent::SetOwner(IEntity* _owner) {
	owner_ = _owner;
}

IEntity* IComponent::GetOwner() const {
	return owner_;
}

