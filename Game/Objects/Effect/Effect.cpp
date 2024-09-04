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
		ImGui::DragFloat("Speed", &speed_, 0.01f);
		ImGui::DragFloat3("GravityModifier", &gravity_.x, 0.01f);
		ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);
		ImGui::DragFloat3("Particle Size", &size_.x, 0.01f, 0.01f, 10.0f);
		ImGui::DragInt("LifeTime", &lifeTime_, 1.0f, 1, 180);

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
			ImGui::SliderFloat("RateTime", &rateDistance_, 0, 40);
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

		}
		if (secondProvisional_ == 1) {
			isCircle_ = false;
			isCone_ = true;
			isBox_ = false;

			xRandomLimite = 1.0f;
			yRamdomLimite = 0.0f;
			zRamdomLimite = 1.0f;

		}
		if (secondProvisional_ == 2) {
			isCircle_ = false;
			isCone_ = false;
			isBox_ = true;

			xRandomLimite = 0.0f;
			yRamdomLimite = 1.0f;
			zRamdomLimite = 0.0f;

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

	float particlesStep = static_cast<float>(appear_) / rateTime_;
	int particlesEmit = static_cast<int>(particlesStep * (rateTime_ - currentRateTime));

	if (currentRateTime > 0) {
		currentRateTime--;
	} else if (currentRateTime == 0) {
		currentRateTime = rateTime_;
	}
	if (particlesEmit > 0) {
		for (int i = 0; i < particlesEmit; ++i) {
			Vector3 newVelo = { Random::Float(-xRandomLimite,xRandomLimite),Random::Float(-yRamdomLimite,yRamdomLimite),Random::Float(-zRamdomLimite,zRamdomLimite) };
			newVelo = newVelo.Normalize() * speed_;
			newVelo = Matrix4x4::TransformNormal(newVelo, rotateEmitter);

			Grain* newGrain = new Grain();
			newGrain->Initialze(model_, transform_.position, rotation_, size_, gravity_, newVelo, lifeTime_, ShiftSpeedType::kNormal,
				shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, SizeChangeType::kReduction);
			grains_.push_back(newGrain);
			currentRateTime = rateTime_;
		}
	}

}
