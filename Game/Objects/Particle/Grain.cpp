#include "Grain.h"


Grain::~Grain() {}

void Grain::Initialize()
{
	drawLayerId = 1;
}

void Grain::InitTrans()
{
	transform_.Initialize();
}

void Grain::Init(Model* model, const Vector3& position, const Vector3& rotate, const Vector3& size,
	Vector3& gravity, const Vector3& velocity, const int life, const ShiftSpeedType shiftType,
	const float shiftSpeed, const bool colorShift, const Vector4& origin,
	const Vector4& change, const bool sizeChange, const Vector3& endSize, SizeChangeType chageType) {


	model_ = model;
	transform_.position = position;
	transform_.rotate = rotate;
	transform_.scale = size;
	gravity_ = gravity;
	velocity_ = velocity;
	lifeTime_ = life;
	startLifeTime_ = life;
	speedType_ = shiftType;
	shiftingSpeed_ = shiftSpeed;
	isColorShift_ = colorShift;
	originColor_ = origin;
	changeColor_ = change;
	isSizeChange_ = sizeChange;
	startScale_ = size;
	endScale_ = endSize;
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
			float t = float(startLifeTime_ - lifeTime_) / (float)startLifeTime_;
			t = t * t;
			if (changeType_ == SizeChangeType::kReduction) {
				transform_.scale = Vector3::Lerp(startScale_, endScale_, t);
			}
			if (changeType_ == SizeChangeType::kExpand) {
				transform_.scale = Vector3::Lerp(startScale_, endScale_, t);
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
