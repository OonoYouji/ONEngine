#include "BaseComponent.h"

#include <BaseGameObject.h>

void BaseComponent::SetParent(BaseGameObject* parent) {
	parent_ = parent;
}
