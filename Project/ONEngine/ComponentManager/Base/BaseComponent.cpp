#include "BaseComponent.h"

#include "GameObjectManager/BaseGameObject.h"

void BaseComponent::SetOwner(BaseGameObject* owner) {
	owner_ = owner;
}

void BaseComponent::SetName(const std::string& name) {
	name_ = name;
}
