#include "Transform.h"


Transform::Transform() {}
Transform::~Transform() {}


void Transform::Update() {
	matWorld_ = Matrix4x4::MakeAffine(position_, rotate_, scale_);

	if (parent_) {
		matWorld_ = parent_->GetMatWorld() * matWorld_;
	}
}
