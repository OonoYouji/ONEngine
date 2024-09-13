#include "Effect.h"
#include <ImGuiManager.h>
#include <WorldTime.h>
#include <Matrix4x4.h>

Effect::~Effect() {

	for (auto& i : grains_)
	{
		i->Destory();
	}
	for (auto& i : grain2Ds_)
	{
		i->SetHaveParent(false);
	}

	grains_.clear();
	grain2Ds_.clear();
}

void Effect::Initialize() {

	emitterName_ = "name";
	if (is3DMode_)
	{
		model_ = ModelManager::Load("HeartBreak");
	}
	transform_.Initialize();
	drawLayerId = 1;
	sprite_.reset(new Sprite());
	if (is2DMode_)
	{
		sprite_->Initialize("uvChecker", "uvChecker.png");
		sprite_->SetSize({ 20.0f,20.0f });
	}
}

void Effect::Reset() {

	grains_.clear();
	grain2Ds_.clear();

}

void Effect::Update() {

	/*grains_.remove_if([](Grain* grain) {
		if (grain->IsDead()) {
			grain->Destory();
			return true;
		}
		return false;
		});
	grain2Ds_.remove_if([](Grain2D* grain) {
		if (grain->IsDead()) {
			grain->Destory();
			return true;
		}
		return false;
		});*/


	transform_.UpdateMatrix();

	if (isStart_) {
		if (is3DMode_) {
			Create();
			previousPosition_ = transform_.position;
		}
		if (is2DMode_) {
			Create2D();
			previousPosition_ = position2D_;
		}
	}

}

void Effect::Draw() {}

void Effect::FrontSpriteDraw() {
}

void Effect::Debug() {

	if (ImGui::TreeNode("Base Variables")) {

		ImGui::SeparatorText("Emitter Variable");
		if (is3DMode_)
		{
			ImGui::DragFloat3("Emitter Position", &transform_.position.x, 0.01f);
			ImGui::DragFloat3("Emitter Roate", &transform_.rotate.x, 0.01f);
		}
		else if (is2DMode_)
		{
			ImGui::DragFloat3("Emitter Position", &position2D_.x, 0.5f);
			ImGui::DragFloat3("Emitter Roate", &transform_.rotate.x, 0.01f);
		}

		ImGui::SeparatorText("Particle Variable");
		ImGui::DragInt("LifeTime", &lifeTime_, 1.0f, 1, 180);
		ImGui::DragFloat("Speed", &speed_, 0.01f);
		ImGui::DragFloat3("GravityModifier", &gravity_.x, 0.01f);
		ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);
		if (is3DMode_)
		{
			ImGui::DragFloat3("Particle Size", &size_.x, 0.1f, 2.0f, 20.0f);
		}
		else if (is2DMode_)
		{
			ImGui::DragFloat3("Particle Size", &size_.x, 0.1f, 2.0f, 20.0f);
		}

		ImGui::SeparatorText("RandomSet Variable");
		ImGui::Checkbox("Random Particle Rotate", &isRotateRandom_); ImGui::SameLine();
		ImGui::Checkbox("Random Particle Size", &isSizeRandom);
		if (isRotateRandom_) {
			ImGui::DragFloat("Min Random Rotate", &minRotateRandom_, 0.01f, -3.0f, 3.0f);
			ImGui::DragFloat("Max Random Rotate", &maxRotateRandom_, 0.01f, -3.0f, 3.0f);
			if (minRotateRandom_ > maxRotateRandom_) {
				minRotateRandom_ = maxRotateRandom_;
			}
		}
		if (isSizeRandom) {
			if (is3DMode_)
			{
				ImGui::DragFloat("Min Random Size", &minSizeRandom_, 0.01f, 0.01f, 2.0f);
				ImGui::DragFloat("Max Random Size", &maxSizeRandom_, 0.01f, 0.01f, 2.0f);
				if (minSizeRandom_ > maxSizeRandom_) {
					minSizeRandom_ = maxSizeRandom_;
				}
			}
			else if (is2DMode_)
			{
				ImGui::DragFloat("Min Random Size", &minSizeRandom_, 0.1f, 1.0f, 10.0f);
				ImGui::DragFloat("Max Random Size", &maxSizeRandom_, 0.1f, 1.0f, 10.0f);
				if (minSizeRandom_ > maxSizeRandom_) {
					minSizeRandom_ = maxSizeRandom_;
				}
			}
		}

		ImGui::TreePop();
	}


	ImGui::SeparatorText("Rate Setting");
	if (ImGui::TreeNode("Rate Type")) {

		ImGui::RadioButton("RateOverTime", &provisional_, 0); ImGui::SameLine();
		ImGui::RadioButton("RateOverDistance", &provisional_, 1);
		if (provisional_ == 0) {
			isOverTime_ = true;
			isOverDistance_ = false;
			ImGui::SliderInt("RateTime", &rateTime_, 0, 120);
			ImGui::SliderInt("Appear Count", &appear_, 1, 40);
		}
		if (provisional_ == 1) {
			isOverTime_ = false;
			isOverDistance_ = true;
			ImGui::SliderFloat("RateDistance", &rateDistance_, 0, 40);
		}

		ImGui::TreePop();
	}


	ImGui::SeparatorText("Emitter Shape");
	if (ImGui::TreeNode("Shape")) {

		ImGui::RadioButton("CirCle", &secondProvisional_, 0); ImGui::SameLine();
		ImGui::RadioButton("Cone", &secondProvisional_, 1); ImGui::SameLine();
		ImGui::RadioButton("Box", &secondProvisional_, 2);
		if (secondProvisional_ == 0) {
			isCircle_ = true;
			isCone_ = false;
			isBox_ = false;

			xRandomLimite = 1.0f;
			yRamdomLimite = 1.0f;
			zRamdomLimite = 0.0f;

			coneLenght_ = 1.0f;

		}
		if (secondProvisional_ == 1) {
			isCircle_ = false;
			isCone_ = true;
			isBox_ = false;

			//ImGui::DragFloat("Corn Lenght", &cornLenght_, 0.01f, 0.0f, 3.0f);
			ImGui::DragFloat("Corn Angle", &coneAngle_, 0.001f, 0.0f, 3.141592f);

			xRandomLimite = 0.0f;
			yRamdomLimite = 0.0f;
			zRamdomLimite = 0.0f;

		}
		if (secondProvisional_ == 2) {
			isCircle_ = false;
			isCone_ = false;
			isBox_ = true;


			if (is3DMode_)
			{
				ImGui::DragFloat("Box Size X", &boxSizeX_, 0.01f, 0.0f, 2.0f);
				ImGui::DragFloat("Box Size Z", &boxSizeZ_, 0.01f, 0.0f, 2.0f);
			}
			else if (is2DMode_)
			{
				ImGui::DragFloat("Box Size X", &boxSizeX_, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat("Box Size Z", &boxSizeZ_, 0.1f, 0.0f, 100.0f);
			}


			xRandomLimite = 0.0f;
			yRamdomLimite = 1.0f;
			zRamdomLimite = 0.0f;

			coneLenght_ = 1.0f;

		}

		ImGui::TreePop();
	}


	ImGui::SeparatorText("Add Parameter");
	if (ImGui::TreeNode("ShiftingSpeed")) {

		ImGui::RadioButton("Normal", &thirdProvisional_, 0); ImGui::SameLine();
		ImGui::RadioButton("Deceleration", &thirdProvisional_, 1); ImGui::SameLine();
		ImGui::RadioButton("Accele", &thirdProvisional_, 2);
		if (thirdProvisional_ == 0) {
			isNormal_ = true;
			isDeceleration_ = false;
			isAccele_ = false;


		}
		if (thirdProvisional_ == 1) {
			isNormal_ = false;
			isDeceleration_ = true;
			isAccele_ = false;

			ImGui::SliderFloat("shift", &shiftingSpeed_, 0.0f, 1.0f);
		}
		if (thirdProvisional_ == 2) {
			isNormal_ = false;
			isDeceleration_ = false;
			isAccele_ = true;

			ImGui::SliderFloat("shift", &shiftingSpeed_, 0.0f, 1.0f);
		}

		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Color Shift")) {

		ImGui::Checkbox("IsColorShift", &isColorShift_);

		if (ImGui::TreeNode("Origin Color")) {
			ImGui::ColorEdit4("Origine", &originalColor_.x, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);
			ImGui::TreePop();
		}
		if (isColorShift_) {
			if (ImGui::TreeNode("Change Color")) {
				ImGui::ColorEdit4("Change", &changeColor_.x, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Size Over")) {

		ImGui::Checkbox("IsSizeOverTime", &isSizeChange_);

		if (isSizeChange_) {
			ImGui::RadioButton("Reduction", &forceProvisional_, 0);
			ImGui::RadioButton("Expand", &forceProvisional_, 1);

			if (is3DMode_)
			{
				ImGui::DragFloat3("EndSize", &endSize_.x, 0.01f, 0.01f, 10.0f);
			}
			else if (is2DMode_)
			{
				ImGui::DragFloat3("EndSize", &endSize_.x, 0.1f, 1.0f, 100.0f);
			}

			if (forceProvisional_ == 0) {
				isReduction_ = true;
				isExpand_ = false;
			}
			if (forceProvisional_ == 1) {
				isReduction_ = false;
				isExpand_ = true;
			}
		}

		ImGui::TreePop();
	}
}


void Effect::Create() {

	Matrix4x4 rotateEmitter = Matrix4x4::MakeRotate(transform_.rotate);


	if (isRotateRandom_) {
		rotation_ = Random::Vec3({ minRotateRandom_,minRotateRandom_ ,minRotateRandom_ }, { maxRotateRandom_,maxRotateRandom_ ,maxRotateRandom_ });
	}
	if (isSizeRandom) {
		float randSize = Random::Float(minSizeRandom_, maxSizeRandom_);
		size_ = { randSize,randSize,randSize };
	}



	if (isOverTime_) {

		float particlesStep = static_cast<float>(appear_) / rateTime_;
		int particlesEmit = static_cast<int>(particlesStep * (rateTime_ - currentRateTime));

		Vector3 newPos = transform_.position;

		if (particlesEmit > 0) {
			for (int i = 0; i < particlesEmit; ++i) {
				if (isBox_) {
					newPos += Matrix4x4::TransformNormal({ Random::Float(-boxSizeX_, boxSizeX_), 0.0f, Random::Float(-boxSizeZ_, boxSizeZ_) }, rotateEmitter);
				}
				Vector3 newVelo;
				if (!isCone_) {
					newVelo = { Random::Float(-xRandomLimite,xRandomLimite),Random::Float(-yRamdomLimite,yRamdomLimite),Random::Float(-zRamdomLimite,zRamdomLimite) };
				}
				else {
					Vector3 randomAngle = { Random::Float(-coneAngle_, coneAngle_) ,0.0f,Random::Float(-coneAngle_, coneAngle_) };
					Matrix4x4 randomRotate = Matrix4x4::MakeRotate(randomAngle);

					Vector3 randomDirection = Matrix4x4::TransformNormal({ 0.0f,1.0f,0.0f }, randomRotate);
					newVelo = randomDirection;
				}
				if (isBox_) {
					newVelo.y = 1.0f;
				}
				newVelo = newVelo.Normalize() * speed_;
				newVelo = Matrix4x4::TransformNormal(newVelo, rotateEmitter);

				if (currentGrainCount <= maxGrainCount)
				{
					Grain* newGrain = new Grain();
					newGrain->Initialize();
					newGrain->InitTrans();
					newGrain->Init(model_, newPos, rotation_, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					grains_.push_back(newGrain);
					currentRateTime = rateTime_;
					currentGrainCount++;
				}
				else
				{
					Grain* newGrain = grains_.front();
					grains_.pop_front();
					newGrain->Init(model_, newPos, rotation_, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					grains_.push_back(newGrain);
					currentRateTime = rateTime_;
					currentGrainCount++;
				}

			}
		}
	}
	else if (isOverDistance_) {

		Vector3 currentPosition = transform_.position;
		float particleDistanceRate = 1.0f / rateDistance_;
		float distanceMoved = Vector3::Length((currentPosition - previousPosition_));
		accumulationDistance += (distanceMoved);
		int particlesEmit = static_cast<int>(accumulationDistance / particleDistanceRate);

		Vector3 newPos = transform_.position;

		if (particlesEmit > 0) {
			for (int i = 0; i < particlesEmit; ++i) {
				if (isBox_) {
					newPos += Matrix4x4::TransformNormal({ Random::Float(-boxSizeX_, boxSizeX_), 0.0f, Random::Float(-boxSizeZ_, boxSizeZ_) }, rotateEmitter);
				}
				Vector3 newVelo = { Random::Float(-xRandomLimite,xRandomLimite),Random::Float(-yRamdomLimite,yRamdomLimite),Random::Float(-zRamdomLimite,zRamdomLimite) };
				if (isCone_ || isBox_) {
					newVelo.y = 1.0f;
				}
				newVelo = newVelo.Normalize() * speed_;
				newVelo = Matrix4x4::TransformNormal(newVelo, rotateEmitter);

				if (currentGrainCount <= maxGrainCount)
				{
					Grain* newGrain = new Grain();
					newGrain->Initialize();
					newGrain->InitTrans();
					newGrain->Init(model_, newPos, rotation_, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					newGrain->SetIsDead(false);
					grains_.push_back(newGrain);
					accumulationDistance = 0.0f;
					currentGrainCount++;
				}
				else
				{
					Grain* newGrain = grains_.front();
					grains_.pop_front();
					newGrain->Init(model_, newPos, rotation_, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					newGrain->SetIsDead(false);
					grains_.push_back(newGrain);
					accumulationDistance = 0.0f;
					currentGrainCount++;
				}
			}
		}
	}



	if (currentRateTime > 0) {
		currentRateTime--;
	}
	else if (currentRateTime == 0) {
		currentRateTime = rateTime_;
	}

}

void Effect::Create2D() {

	Vector3 newEmitterRotate = { 0.0f,0.0f,transform_.rotate.z };
	Matrix4x4 rotateEmitter = Matrix4x4::MakeRotate(newEmitterRotate);
	Vector3 newRotateSpeed{};

	if (isRotateRandom_) {
		rotation_ = Random::Vec3({ minRotateRandom_,minRotateRandom_ ,minRotateRandom_ }, { maxRotateRandom_,maxRotateRandom_ ,maxRotateRandom_ });
		newRotateSpeed = Random::Vec3({ minRotateSpeed_,minRotateSpeed_ ,minRotateSpeed_ }, { maxRotateSpeed_,maxRotateSpeed_ ,maxRotateSpeed_ });
	}
	if (isSizeRandom) {
		float randSize = Random::Float(minSizeRandom_, maxSizeRandom_);
		size_ = { randSize,randSize,randSize };
	}

	if (isNormal_) {
		speedType = ShiftSpeedType::kNormal;
	}
	if (isDeceleration_) {
		speedType = ShiftSpeedType::kDeceleration;
	}
	if (isAccele_) {
		speedType = ShiftSpeedType::kAccele;
	}


	if (isOverTime_) {

		float particlesStep = static_cast<float>(appear_) / rateTime_;
		int particlesEmit = static_cast<int>(particlesStep * (rateTime_ - currentRateTime));

		Vector3 newPos = position2D_;

		if (particlesEmit > 0) {
			for (int i = 0; i < particlesEmit; ++i) {
				if (isBox_) {
					newPos += Matrix4x4::TransformNormal({ Random::Float(-boxSizeX_, boxSizeX_), 0.0f, Random::Float(-boxSizeZ_, boxSizeZ_) }, rotateEmitter);
				}
				Vector3 newVelo;
				if (!isCone_) {
					newVelo = { Random::Float(-xRandomLimite,xRandomLimite),Random::Float(-yRamdomLimite,yRamdomLimite),Random::Float(-zRamdomLimite,zRamdomLimite) };
				}
				else {
					Vector3 randomAngle = { Random::Float(-coneAngle_, coneAngle_) ,0.0f,Random::Float(-coneAngle_, coneAngle_) };
					Matrix4x4 randomRotate = Matrix4x4::MakeRotate(randomAngle);

					Vector3 randomDirection = Matrix4x4::TransformNormal({ 0.0f,1.0f,0.0f }, randomRotate);
					newVelo = randomDirection;
				}
				if (isBox_) {
					newVelo.y = 1.0f;
				}
				newVelo = newVelo.Normalize() * speed_;
				newVelo = Matrix4x4::TransformNormal(newVelo, rotateEmitter);

				if (currentGrainCount <= maxGrainCount)
				{
					Grain2D* newGrain = new Grain2D();
					newGrain->Initialize();
					newGrain->Init(newPos, rotation_, newRotateSpeed, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					newGrain->SetIsDead(false);
					newGrain->SetHaveParent(true);
					grain2Ds_.push_back(newGrain);
					currentRateTime = rateTime_;
					currentGrainCount++;
				}
				else
				{
					Grain2D* newGrain = grain2Ds_.front();
					grain2Ds_.pop_front();
					newGrain->Init(newPos, rotation_, newRotateSpeed,size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					newGrain->SetIsDead(false);
					newGrain->SetHaveParent(true);
					grain2Ds_.push_back(newGrain);
					currentRateTime = rateTime_;
					currentGrainCount++;
				}
			}
		}
	}
	else if (isOverDistance_) {

		Vector3 currentPosition = position2D_;
		float particleDistanceRate = 20.0f / rateDistance_;
		float distanceMoved = Vector3::Length((currentPosition - previousPosition_));
		if (distanceMoved > 200.0f) {
			distanceMoved = 200.0f;
		}
		accumulationDistance += distanceMoved;
		int particlesEmit = static_cast<int>(accumulationDistance / particleDistanceRate);

		Vector3 newPos = position2D_;

		if (particlesEmit > 0) {
			for (int i = 0; i < particlesEmit; ++i) {
				if (isBox_) {
					newPos += Matrix4x4::TransformNormal({ Random::Float(-boxSizeX_, boxSizeX_), 0.0f, Random::Float(-boxSizeZ_, boxSizeZ_) }, rotateEmitter);
				}
				Vector3 newVelo = { Random::Float(-xRandomLimite,xRandomLimite),Random::Float(-yRamdomLimite,yRamdomLimite),Random::Float(-zRamdomLimite,zRamdomLimite) };
				if (isCone_ || isBox_) {
					newVelo.y = 1.0f;
				}
				newVelo = newVelo.Normalize() * speed_;
				newVelo = Matrix4x4::TransformNormal(newVelo, rotateEmitter);

				if (currentGrainCount <= maxGrainCount)
				{
					Grain2D* newGrain = new Grain2D();
					newGrain->Initialize();
					newGrain->Init(newPos, rotation_, newRotateSpeed, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					newGrain->SetIsDead(false);
					newGrain->SetHaveParent(true);
					grain2Ds_.push_back(newGrain);
					currentRateTime = rateTime_;
					accumulationDistance = 0.0f;
					currentGrainCount++;
				}
				else
				{
					Grain2D* newGrain = grain2Ds_.front();
					grain2Ds_.pop_front();
					newGrain->Init(newPos, rotation_, newRotateSpeed, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
						shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, endSize_, SizeChangeType::kReduction);
					newGrain->SetIsDead(false);
					newGrain->SetHaveParent(true);
					grain2Ds_.push_back(newGrain);
					currentRateTime = rateTime_;
					accumulationDistance = 0.0f;
					currentGrainCount++;
				}
			}
		}
	}



	if (currentRateTime > 0) {
		currentRateTime--;
	}
	else if (currentRateTime == 0) {
		currentRateTime = rateTime_;
	}

}

void Effect::EffectStart()
{
	isStart_ = true;
}

void Effect::EffectStop()
{
	isStart_ = false;
}

void Effect::SetGrainMode(int type)
{

	if (type == 0)
	{
		is3DMode_ = true;
		is2DMode_ = false;
	}
	else if (type == 1)
	{
		is3DMode_ = false;
		is2DMode_ = true;
	}

}

void Effect::SetOverType(int type)
{
	if (type == 0)
	{
		isOverTime_ = true;
		isOverDistance_ = false;
		provisional_ = 0;
	}
	else if (type == 1) {
		isOverDistance_ = true;
		isOverTime_ = false;
		provisional_ = 1;
	}
}

void Effect::OverTimeSetting(int rateTime, int appear)
{
	rateTime_ = rateTime;
	appear_ = appear;
}

void Effect::OverDistanceSetting(float rateDistance)
{
	rateDistance_ = rateDistance;
}

void Effect::ShapeType(int type)
{
	if (type == 0)
	{
		isCircle_ = true;
		isCone_ = false;
		isBox_ = false;
		secondProvisional_ = 0;
	}
	else if (type == 1) {
		isCircle_ = false;
		isCone_ = true;
		isBox_ = false;
		secondProvisional_ = 1;
	}
	else if (type == 2) {
		isCircle_ = false;
		isCone_ = false;
		isBox_ = true;
		secondProvisional_ = 2;
	}
}

void Effect::SetBoxSize(float xSize, float zSize) {
	boxSizeX_ = xSize;
	boxSizeZ_ = zSize;
}

void Effect::ShiftingSpeedType(int type)
{
	if (type == 0)
	{
		isNormal_ = true;
		isDeceleration_ = false;
		isAccele_ = false;
		thirdProvisional_ = 0;
	}
	else if (type == 1)
	{
		isNormal_ = false;
		isDeceleration_ = true;
		isAccele_ = false;
		thirdProvisional_ = 1;
	}
	else if (type == 2)
	{
		isNormal_ = false;
		isDeceleration_ = false;
		isAccele_ = true;
		thirdProvisional_ = 2;
	}
}

void Effect::ShiftSpeedSetting(float shiftSpeed)
{
	shiftingSpeed_ = shiftSpeed;
}

void Effect::SizeChangeSetting(bool sizeChange, bool isReduction, bool isExpand, Vector3 endSize)
{
	isSizeChange_ = sizeChange;
	isReduction_ = isReduction;
	isExpand_ = isExpand;
	endSize_ = endSize;
}


void Effect::SetPos(const Vector3& pos)
{
	transform_.position = pos;
}

void Effect::SetPos(const Vector2& pos)
{
	position2D_ = { pos,0.0f };
}

void Effect::SetVariavles2D(const Vector3& gravity, float speed, const Vector3& size, int lifeTime, bool rotateRandom, bool sizeRandom, float minSizeRandom, float maxSizeRandom)
{
	gravity_ = gravity;
	speed_ = speed;
	size_ = size;
	lifeTime_ = lifeTime;
	isRotateRandom_ = rotateRandom;
	isSizeRandom = sizeRandom;
	minSizeRandom_ = minSizeRandom;
	maxSizeRandom_ = maxSizeRandom;

}

void Effect::SetVariavles(const Vector3& gravity, float speed, const Vector3& size, int lifeTime, bool rotateRandom, bool sizeRandom, float minSizeRandom, float maxSizeRandom)
{

	gravity_ = gravity;
	speed_ = speed;
	size_ = size;
	lifeTime_ = lifeTime;
	isRotateRandom_ = rotateRandom;
	isSizeRandom = sizeRandom;
	minSizeRandom_ = minSizeRandom;
	maxSizeRandom_ = maxSizeRandom;


}
