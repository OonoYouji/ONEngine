#include "ICollider.h"

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"

void ICollider::UpdatePrevPosition() {
	/// 前フレームの座標を更新する
	if (GameEntity* owner = GetOwner()) {
		prevPosition_ = owner->GetTransform()->GetPosition();
	}
}

const Vector3& ICollider::GetPrevPosition() const {
	return prevPosition_;
}

CollisionState ICollider::GetCollisionState() const {
	return collisionState_;
}

