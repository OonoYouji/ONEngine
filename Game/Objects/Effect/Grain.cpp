#include "Grain.h"

Grain::Grain() {}

Grain::~Grain() {}

void Grain::Initialze(Model* model, const Transform& trans, Vector3& gravity,
	const Vector3& velocity, const int life, const ShiftSpeedType shiftType,
	const float shiftSpeed, const bool colorShift, const Vector4& origin,
	const Vector4& change, const bool sizeChange, SizeChangeType chageType) {

	transform_.Initialize();

	model_ = model;
	transform_ = trans;
	gravity_ = gravity;
	velocity_ = velocity;
	lifeTime_ = life;
	speedType_ = shiftType;
	shiftingSpeed_ = shiftSpeed;
	isColorShift_ = colorShift;
	originColor_ = origin;
	changeColor_ = change;
	isSizeChange_ = sizeChange;
	changeType_ = chageType;

}

void Grain::Update() {

	if (lifeTime_ > 0) {
		lifeTime_--;

		if (speedType_ == ShiftSpeedType::kNormal) {

		}
		if (speedType_ == ShiftSpeedType::kDeceleration) {

		}
		if (speedType_ == ShiftSpeedType::kAccele) {

		}
		if (isColorShift_) {

		}
		if (isSizeChange_) {
			if (changeType_ == SizeChangeType::kReduction){

			}
			if (changeType_ == SizeChangeType::kExpand) {

			}
		}


		velocity_ += gravity_;
		transform_.position += velocity_;
		transform_.UpdateMatrix();

	}
	else if (lifeTime_ == 0) {
		isDead_ = true;
	}

}

void Grain::Draw() {
	model_->Draw(&transform_);
}
