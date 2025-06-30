#include "ICollider.h"

void ICollider::SetOnCollisionEnterCallBackFunc(ColliderCallBackFunc _func) {
	onCollisionEnterCallBackFunc_ = _func;
}

void ICollider::SetOnCollisionExitCallBackFunc(ColliderCallBackFunc _func) {
	onCollisionExitCallBackFunc_ = _func;
}

void ICollider::SetOnCollisionStayCallBackFunc(ColliderCallBackFunc _func) {
	onCollisionStayCallBackFunc_ = _func;
}
