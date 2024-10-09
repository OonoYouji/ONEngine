#include "FrameFixation.h"

#include <thread>
#include <format>

#include "ImGuiManager/ImGuiManager.h"


void FrameFixation::Initialize(bool isFixation, uint32_t maxFrameRate) {
	isFixation_ = isFixation;

	maxFrameRate_  = maxFrameRate;	/// 最大FPS
	minEspsetTime_ = 1000000ll / static_cast<long long>(maxFrameRate_);

}

void FrameFixation::ImGuiDebug() {
	if(!ImGui::Begin("FrameFixation")) {
		ImGui::End();
		return;
	}

	/// 固定化フラグ
	ImGui::Checkbox("isFixation", &isFixation_);

	ImGui::Text(std::format("espset time = {}", minEspsetTime_).c_str());


	ImGui::End();
}

void FrameFixation::Fixation() {
	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float, std::milli> duration   = end - currentTime_;
	std::chrono::microseconds                espsedTime = std::chrono::duration_cast<std::chrono::microseconds>(duration);

	if(isFixation_) {
	
		/// fpsの固定時間より速く1フレーム終了したので待機する
		long long waitTime = minEspsetTime_ - espsedTime.count();
	
		if(waitTime > 0.0f) {
			/// 実行を待つ時間を計算、待つ
			std::chrono::microseconds sleepDuration(waitTime);
			std::this_thread::sleep_for(sleepDuration);
		}
	
	}

	/// 現在の時間を取得
	currentTime_ = std::chrono::high_resolution_clock::now();

}
