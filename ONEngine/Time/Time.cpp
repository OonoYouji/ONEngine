#include "Time.h"

#include <ImGuiManager.h>


Time Time::sInstance_;



void Time::Update() {
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - time_;
	time_ = std::chrono::high_resolution_clock::now();

	deltaTime_ = duration.count() / 1000.0f;
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