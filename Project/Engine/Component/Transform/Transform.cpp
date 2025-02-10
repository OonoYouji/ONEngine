#include "Transform.h"


Transform::Transform() {}
Transform::~Transform() {}


void Transform::Update() {
	matWorld = Matrix4x4::MakeAffine(position, rotate, scale);

	if (parent_) {
		matWorld = parent_->GetMatWorld() * matWorld;
	}
}

