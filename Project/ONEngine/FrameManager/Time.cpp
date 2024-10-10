#include "Time.h"

#include <format>

#include "ImGuiManager/ImGuiManager.h"


Time Time::sInstance_;



void Time::Update() {
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - time_;
	time_ = std::chrono::high_resolution_clock::now();

	deltaTime_ = duration.count() / 1000.0f;

	exeTimes_.push_back(duration.count());
	if(exeTimes_.size() > 120) {
		exeTimes_.pop_front();
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

	ImVec2 winSize = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("execution time", winSize, 0, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	for(uint32_t i = 0u; i < uint32_t(exeTimes_.size()); ++i) {
		ImGui::Text(std::format("time[{}] = {} mill second", i, exeTimes_[i]).c_str());
	}

	ImGui::EndChild();


	ImGui::End();
#endif // _DEBUG
}
