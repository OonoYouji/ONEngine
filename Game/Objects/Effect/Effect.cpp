#include "Effect.h"
#include <ImGuiManager.h>

Effect::Effect() {}

Effect::~Effect() {

	for (Grain* grain : grains_) {
		delete grain;
	}

}

void Effect::Initialize(const std::string& name) {

	emitterName_ = name;
	model_ = ModelManager::CreateCube();
	model_->Initialize();
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

		ImGui::DragFloat3("Direction", &direction_.x, 0.01f);
		ImGui::DragFloat3("GravityModifier", &gravity_.x, 0.01f);
		ImGui::DragFloat3("Rotation", &rotation_.x, 0.01f);
		ImGui::DragFloat3("Particle Size", &size_.x, 0.01f);
		ImGui::DragInt("LifeTime", &lifeTime_, 1.0f, 0, 180);

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
			ImGui::SliderInt("Appear Count", &appear_, 0, 40);
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
		ImGui::RadioButton("Sphere", &secondProvisional_, 2);
		if (secondProvisional_ == 0) {
			isCircle_ = true;
			isCone_ = false;
			isSphere_ = false;

			// エミッターの形のサイズを編集できるように

		}
		if (secondProvisional_ == 1) {
			isCircle_ = false;
			isCone_ = true;
			isSphere_ = false;

			// エミッターの形のサイズを編集できるように

		}
		if (secondProvisional_ == 2) {
			isCircle_ = false;
			isCone_ = false;
			isSphere_ = true;

			// エミッターの形のサイズを編集できるように

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

	Grain* newGrain = new Grain();
	newGrain->Initialze(model_, transform_, gravity_, direction_, lifeTime_, ShiftSpeedType::kNormal,
		shiftingSpeed_, isColorShift_, originalColor_, changeColor_, isSizeChange_, SizeChangeType::kReduction);
	grains_.push_back(newGrain);

}

//void Effect::Draw(const ViewProjection& viewProjection) {}
//
//void Effect::CreateWalk(const Vector3& postion) {}
//
//void Effect::CreateEnemyDeth(const Vector3& position) {}
//
//void Effect::CreateSnowBallDeth(const Vector3& position) {}
