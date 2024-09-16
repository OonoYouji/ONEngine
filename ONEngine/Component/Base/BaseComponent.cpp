#include "BaseComponent.h"

#include <BaseGameObject.h>

void BaseComponent::SetParent(BaseGameObject* parent) {
	parent_ = parent;
}

void BaseComponent::SetName(const std::string& name) {
	name_ = name;
}
