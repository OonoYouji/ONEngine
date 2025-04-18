#include "Transform.h"


Transform::Transform() {
	position = Vector3::kZero;
	rotate   = Vector3::kZero;
	scale    = Vector3::kOne;
}
Transform::~Transform() {}


void Transform::Update() {
	matWorld = Matrix4x4::MakeAffine(scale, rotate, position);

	if (parent_) {

		if ((matrixCalcFlags & kAll) == kAll) {
			matWorld *= parent_->GetMatWorld();
			return;
		}

		Matrix4x4 matCancel = Matrix4x4::kIdentity;
		if (matrixCalcFlags & kScale) {
			matCancel = Matrix4x4::MakeScale(parent_->scale);
		}

		if (matrixCalcFlags & kRotate) {
			matCancel *= Matrix4x4::MakeRotate(parent_->rotate);
		}

		if (matrixCalcFlags & kPosition) {
			matCancel *= Matrix4x4::MakeTranslate(parent_->position);
		}

		matWorld *= matCancel;

	}
}

