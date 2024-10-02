#include "BaseCollider.h"

#include <GameObjectManager.h>

void BaseCollider::UpdateMatrix() {
	transform_->UpdateMatrix();
	transform_->matTransform *= GetParent()->GetMatTransform();
}

Vec3 BaseCollider::GetPosition() const {
	return GetParent()->GetPosition();
}
