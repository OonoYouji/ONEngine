#include "Shake.h"

#include <ImGuiManager.h>
#include <WorldTime.h>

void Shake::Initialize() {
	isDrawActive = false;
	time_ = 0.0f;
	speed_ = 0.1f;
	startTime_ = 1.0f;
}

void Shake::Update() {
	if(time_ <= 0.0f) {
		transform_.position = { 0,0,0 };
		return;
	}
	time_ -= WorldTime::DeltaTime();
	auto lRandom = []() -> float {return (rand() % 20 - 9) / 10.0f; };
	Vec3 randomV = { lRandom(), lRandom(),lRandom() };
	transform_.position = randomV * speed_;
	transform_.position.z = 0.0f;
	UpdateMatrix();
}

void Shake::Debug() {
	if(ImGui::TreeNodeEx("shake", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("startTime", &startTime_, 0.01f);
		ImGui::DragFloat("time", &time_, 0.01f);
		ImGui::DragFloat("speed", &speed_, 0.01f);
		ImGui::TreePop();
	}
}

void Shake::Start() {
	time_ = startTime_;
}

void Shake::SetStartTime(float time) {
	startTime_ = time;
}
