#include "Effect.h"
#include <ImGuiManager.h>
#include <Matrix4x4.h>

Effect::Effect() {}

Effect::~Effect() {

	for (Grain* grain : grains_) {
		delete grain;
	}

}

void Effect::Initialize(const std::string& name) {

	emitterName_ = name;
	model_ = ModelManager::CreateCube();
	transform_.Initialize();

}

void Effect::Reset() {

	for (Grain* grain : grains_) {
		delete grain;
	}

	grains_.clear();

}

void Effect::Update() {

	grains_.remove_if([](Grain* grain) {
	if (grain->IsDead()) {
		delete grain;
		return true;
	}
	return false;
	});

	previousPosition_ = transform_.position;

	Setting();
	transform_.UpdateMatrix();

	Create();

	for (Grain* grain : grains_) {
		grain->Update();
	}

}

void Effect::Draw() {

	for (Grain* grain : grains_) {
		grain->Draw();
	}

}

void Effect::Setting(){
#ifdef _DEBUG

	ImGui::Begin(emitterName_.c_str());

	if (ImGui::TreeNode("Base Variables")) {

		ImGui::SeparatorText("Emitter Variable");
		ImGui::DragFloat3("Emitter Position", &transform_.position.x, 0.01f);
		ImGui::DragFloat3("Emitter Roate", &transform_.rotate.x, 0.01f);

		ImGui::SeparatorText("Particle Variable");
		ImGui::DragInt("LifeTime", &lifeTime_, 1.0f, 1, 180);
		ImGui::DragFloat("Speed", &speed_, 0.01f);
		ImGui::DragFloat3("GravityModifier", &gravity_.x, 0.01f);
		ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);
		ImGui::DragFloat3("Particle Size", &size_.x, 0.01f, 0.01f, 10.0f);

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
			ImGui::DragFloat("Min Random Size", &minSizeRandom_, 0.01f, 0.01f, 2.0f);
			ImGui::DragFloat("Max Random Size", &maxSizeRandom_, 0.01f, 0.01f, 2.0f);
			if (minSizeRandom_ > maxSizeRandom_) {
				minSizeRandom_ = maxSizeRandom_;
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

			cornLenght_ = 1.0f;

		}
		if (secondProvisional_ == 1) {
			isCircle_ = false;
			isCone_ = true;
			isBox_ = false;

			ImGui::DragFloat("Corn Lenght", &cornLenght_, 0.01f, 0.0f, 3.0f);

			xRandomLimite = 1.0f;
			yRamdomLimite = 0.0f;
			zRamdomLimite = 1.0f;

		}
		if (secondProvisional_ == 2) {
			isCircle_ = false;
			isCone_ = false;
			isBox_ = true;

			ImGui::DragFloat("Box Size X", &boxSizeX, 0.01f, 0.0f, 2.0f);
			ImGui::DragFloat("Box Size Z", &boxSizeZ, 0.01f, 0.0f, 2.0f);

			xRandomLimite = 0.0f;
			yRamdomLimite = 1.0f;
			zRamdomLimite = 0.0f;

			cornLenght_ = 1.0f;

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


	ImGui::End();

#endif // _DEBUG
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
					newPos += Matrix4x4::TransformNormal({ Random::Float(-boxSizeX, boxSizeX), 0.0f, Random::Float(-boxSizeZ, boxSizeZ) }, rotateEmitter);
				}
				Vector3 newVelo = { Random::Float(-xRandomLimite,xRandomLimite),Random::Float(-yRamdomLimite,yRamdomLimite),Random::Float(-zRamdomLimite,zRamdomLimite) };
				if (isCone_ || isBox_) {
					newVelo.y = 1.0f;
				}
				newVelo = newVelo.Normalize() * speed_;
				if (isCone_) {
					newVelo.y = newVelo.y * cornLenght_;
				}
				newVelo = Matrix4x4::TransformNormal(newVelo, rotateEmitter);

				Grain* newGrain = new Grain();
				newGrain->Initialze(model_, newPos, rotation_, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
					shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, SizeChangeType::kReduction);
				grains_.push_back(newGrain);
				currentRateTime = rateTime_;
			}
		}
	}
	else if (isOverDistance_) {

		Vector3 currentPosition = transform_.position;
		float particleDistanceRate = 1.0f / rateDistance_;
		float distanceMoved = Vector3::Length((currentPosition - previousPosition_));
		accumulationDistance += distanceMoved;
		int particlesEmit = static_cast<int>(accumulationDistance / particleDistanceRate);

		Vector3 newPos = transform_.position;

		if (particlesEmit > 0) {
			for (int i = 0; i < particlesEmit; ++i) {
				if (isBox_) {
					newPos += Matrix4x4::TransformNormal({ Random::Float(-boxSizeX, boxSizeX), 0.0f, Random::Float(-boxSizeZ, boxSizeZ) }, rotateEmitter);
				}
				Vector3 newVelo = { Random::Float(-xRandomLimite,xRandomLimite),Random::Float(-yRamdomLimite,yRamdomLimite),Random::Float(-zRamdomLimite,zRamdomLimite) };
				if (isCone_ || isBox_) {
					newVelo.y = 1.0f;
				}
				newVelo = newVelo.Normalize() * speed_;
				if (isCone_) {
					newVelo.y = newVelo.y * cornLenght_;
				}
				newVelo = Matrix4x4::TransformNormal(newVelo, rotateEmitter);

				Grain* newGrain = new Grain();
				newGrain->Initialze(model_, newPos, rotation_, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
					shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, SizeChangeType::kReduction);
				grains_.push_back(newGrain);
				accumulationDistance = 0.0f;
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
