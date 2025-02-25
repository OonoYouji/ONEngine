#include "IEntity.h"

IEntity::IEntity() {
	transform_ = AddComponent<Transform>();
}

void IEntity::SetParent(IEntity* _parent) {
	transform_->SetParent(_parent->GetTransform());
}
