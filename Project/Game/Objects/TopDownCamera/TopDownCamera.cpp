#define NOMINMAX
#include "TopDownCamera.h"

/// externals
#include <imgui.h>

/// engine
#include "VariableManager/VariableManager.h"
#include "FrameManager/Time.h"
#include "Math/Random.h"

/// game
#include "Objects/Camera/GameCamera.h"


TopDownCamera::TopDownCamera(class GameCamera* _gameCamera, BaseGameObject* _targetObject)
	: pGameCamera_(_gameCamera), targetObject_(_targetObject) {

	CreateTag(this);
}

TopDownCamera::~TopDownCamera() {}

void TopDownCamera::Initialize() {
	pGameCamera_->SetParent(pTransform_);

	shakeValue_ = {};

	AddVariables();
	VariableManager::GetInstance()->LoadSpecificGroupsToJson(
		"./Resources/Parameters/Objects", GetTag()
	);
	ApplyVariables();
}

void TopDownCamera::Update() {
	ApplyVariables();

	if(shakeTime_ <= shakeMaxTime_) {
		shakeTime_ += Time::DeltaTime();

		float lerpT = std::min(shakeTime_ / shakeMaxTime_, 1.0f);
		float value = std::lerp(shakeMaxValue_, shakeMinValue_, lerpT);

		shakeValue_ = Random::Vec3(-Vec3::kOne, Vec3::kOne) * value;

		if(shakeTime_ >= shakeMaxTime_) {
			shakeValue_ = { 0,0,0 };
		}
	}


	pGameCamera_->SetPosition(offsetPosition_ + shakeValue_);
	pGameCamera_->SetRotate(offsetRotate_);

	pTransform_->position = targetObject_->GetPosition();
}

void TopDownCamera::Debug() {
	{
		static float minV, maxV, time;
		ImGui::DragFloat("min", &minV, 0.1f);
		ImGui::DragFloat("max", &maxV, 0.1f);
		ImGui::DragFloat("time", &time, 0.1f);

		if(ImGui::Button("shake")) {
			StartShake(minV, maxV, time);
		}
	}
}



void TopDownCamera::AddVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	vm->AddValue(groupName, "offsetPosition", offsetPosition_);
	vm->AddValue(groupName, "offsetRotate", offsetRotate_);
}

void TopDownCamera::ApplyVariables() {
	VariableManager* vm = VariableManager::GetInstance();
	const std::string& groupName = GetTag();

	offsetPosition_ = vm->GetValue<Vec3>(groupName, "offsetPosition");
	offsetRotate_ = vm->GetValue<Vec3>(groupName, "offsetRotate");
}



void TopDownCamera::StartShake(float _minValue, float _maxValue, float _time) {
	/// 量
	shakeMinValue_ = _minValue;
	shakeMaxValue_ = _maxValue;

	/// 時間
	shakeMaxTime_ = _time;
	shakeTime_ = 0.0f;
}

