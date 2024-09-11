#include "Grain2D.h"


Grain2D::~Grain2D() {}

void Grain2D::Initialize()
{
	drawLayerId = 0;
}

void Grain2D::Init(const Vector3& position, const Vector3& rotate, const Vector3& rotateSpeed, const Vector3& size, Vector3& gravity, const Vector3& velocity, const int life,
	const ShiftSpeedType shiftType, const float shiftSpeed, const bool colorShift, const Vector4& origin,
	const Vector4& change, const bool sizeChange, const Vector3& endSize, SizeChangeType chageType) {

	if (!sprite_)
	{
		sprite_.reset(new Sprite());
		sprite_->Initialize("particle", "particle.png");
		sprite_->SetSize({ 20.0f,20.0f });
	}

	position_ = position;
	theta_ = rotate;
	rotateSpeed_ = rotateSpeed;
	size_ = { size.x,size.y };
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

	sprite_->SetSize(size_);

}

void Grain2D::Update() {

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
				Vector3 newVec3 = Vector3::Lerp(startScale_, endScale_, t);
				size_ = { newVec3.x, newVec3.y };
				sprite_->SetSize(size_);
			}
			if (changeType_ == SizeChangeType::kExpand) {
				Vector3 newVec3 = Vector3::Lerp(startScale_, endScale_, t);
				size_ = { newVec3.x, newVec3.y };
				sprite_->SetSize(size_);
			}
		}

		theta_.z += rotateSpeed_.z;
		sprite_->SetAngle(theta_.z);
		velocity_ += gravity_;
		position_ += velocity_;
		sprite_->SetPos(position_);
	}
	else if (lifeTime_ == 0) {
		isDead_ = true;
	}

}

void Grain2D::Draw() {}

void Grain2D::FrontSpriteDraw()
{
	if (!isDead_)
	{
		sprite_->Draw();
	}
}
