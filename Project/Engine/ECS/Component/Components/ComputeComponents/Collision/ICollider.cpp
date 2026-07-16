#include "ICollider.h"

/// engine
#include "Engine/ECS/Entity/GameEntity/GameEntity.h"


namespace ONEngine {

/**
 * @brief 前フレーム位置（prevPosition_）を現在の位置で更新します。
 */
void ICollider::UpdatePrevPosition() {
	if(GameEntity* owner = GetOwner()) {
		prevPosition_ = owner->GetTransform()->GetPosition();
	}
}

/**
 * @brief 前フレームのワールド座標位置を取得します。
 */
const Vector3& ICollider::GetPrevPosition() const {
	return prevPosition_;
}

/**
 * @brief コライダーの動的/静的状態（CollisionState）を取得します。
 */
CollisionState ICollider::GetCollisionState() const {
	return collisionState_;
}



} /// namespace ONEngine