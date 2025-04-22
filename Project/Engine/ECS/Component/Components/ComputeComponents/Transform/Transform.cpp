#include "Transform.h"


Transform::Transform() {
	position = Vector3::kZero;
	rotate   = Vector3::kZero;
	scale    = Vector3::kOne;
}
Transform::~Transform() {}


void Transform::Update() {
	matWorld = Matrix4x4::MakeAffine(scale, rotate, position);
}

