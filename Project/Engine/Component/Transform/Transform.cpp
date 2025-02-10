#include "Transform.h"


Transform::Transform() {}
Transform::~Transform() {}


void Transform::Update() {
	matWorld = Matrix4x4::MakeAffine(scale, rotate, position);

	if (parent_) {
		matWorld = parent_->GetMatWorld() * matWorld;
	}
}

