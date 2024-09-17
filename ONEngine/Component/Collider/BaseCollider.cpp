#include "BaseCollider.h"

#include <GameObjectManager.h>

void BaseCollider::UpdateMatrix() {
	transform_->UpdateMatrix();
	transform_->matTransform *= GetParent()->GetMatTransform();
}