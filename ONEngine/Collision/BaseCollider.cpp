#include "BaseCollider.h"

#include <GameObjectManager.h>
#include <Model.h>

void BaseCollider::Initialize(BaseGameObject* gameObject, [[maybe_unused]] Model* model) {
	gameObject_ = gameObject;
}

void BaseCollider::UpdateMatrix() {
	transform_.UpdateMatrix();
	transform_.matTransform *= gameObject_->GetMatTransform();
}
