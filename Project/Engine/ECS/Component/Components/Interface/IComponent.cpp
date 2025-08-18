#include "IComponent.h"

void IComponent::SetOwner(GameEntity* _owner) {
	owner_ = _owner;
}

GameEntity* IComponent::GetOwner() const {
	return owner_;
}

