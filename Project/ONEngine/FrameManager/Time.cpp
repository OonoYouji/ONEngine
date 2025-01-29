#include "Time.h"

#include <format>

#include "ImGuiManager/ImGuiManager.h"


Time Time::sInstance_;



void Time::SetTimeRate(float _timeRate) {
	sInstance_.timeRate_ = _timeRate;
}

void Time::SetTimeRate(float _timeRate, float _limit) {
	sInstance_.timeRate_      = _timeRate;
	sInstance_.timeRateLimit_ = _limit;
}

void Time::Update() {
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - time_;
	time_ = std::chrono::high_resolution_clock::now();

	deltaTime_ = duration.count() / 1000.0f;

	if(timeRateLimit_ > 0.0f) {
		timeRateLimit_ -= deltaTime_;
		if(timeRateLimit_ <= 0.0f) {
			timeRate_ = 1.0f;
		}
	}
}


void Time::ImGuiDebug() {
#ifdef _DEBUG
	if(!ImGui::Begin("Time")) {
		ImGui::End();
		return;
	}

	ImGui::DragFloat("delta time", &deltaTime_, 0.0f);

	float fps = 1.0f / deltaTime_;
	ImGui::DragFloat("fps", &fps, 0.0f);


	ImGui::End();
#endif // _DEBUG
}
