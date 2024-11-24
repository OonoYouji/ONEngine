#include "BaseCollider.h"

#include "GameObjectManager/GameObjectManager.h"

void BaseCollider::UpdateMatrix() {
	transform_->UpdateMatrix();
	transform_->matTransform *= GetOwner()->GetMatTransform();
}

Vec3 BaseCollider::GetPosition() const {
	return GetOwner()->GetPosition();
}

void BaseCollider::SetIsDrawCollider(bool _isCollider) {
	isDrawCollider_ = _isCollider;
}
