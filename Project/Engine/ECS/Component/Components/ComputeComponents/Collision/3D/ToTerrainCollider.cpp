#include "ToTerrainCollider.h"

void ToTerrainCollider::SetIsCollided(bool _isCollided) {
	isCollided_ = _isCollided;
}

bool ToTerrainCollider::GetIsCollided() const {
	return isCollided_;
}

const Vector3& ToTerrainCollider::GetPrevPosition() const {
	return prevPosition_;
}
