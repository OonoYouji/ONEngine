#include "SphereCollider.h"

SphereCollider::SphereCollider() {
	// デフォルトの値をセット
	radius_ = 1.0f; 
}

void SphereCollider::SetRadius(float _radius) {
	radius_ = _radius;
}

float SphereCollider::GetRadius() const {
	return radius_;
}
