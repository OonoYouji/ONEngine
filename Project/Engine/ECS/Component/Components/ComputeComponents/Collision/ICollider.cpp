#include "ICollider.h"

/// engine
#include "Engine/ECS/Entity/Interface/IEntity.h"

void ICollider::UpdatePrevPosition() {
	/// 前フレームの座標を更新する
	if (GameEntity* owner = GetOwner()) {
		prevPosition_ = owner->GetTransform()->GetPosition();
	}
}

const Vector3& ICollider::GetPrevPosition() const {
	return prevPosition_;
}

void ICollider::SetOnCollisionEnterCallBackFunc(ColliderCallBackFunc _func) {
	onCollisionEnterCallBackFunc_ = _func;
}

void ICollider::SetOnCollisionExitCallBackFunc(ColliderCallBackFunc _func) {
	onCollisionExitCallBackFunc_ = _func;
}

void ICollider::SetOnCollisionStayCallBackFunc(ColliderCallBackFunc _func) {
	onCollisionStayCallBackFunc_ = _func;
}
