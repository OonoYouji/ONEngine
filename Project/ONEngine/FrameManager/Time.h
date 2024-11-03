#pragma once

#include <chrono>
#include <deque>

/// ===================================================
/// 時間
/// ===================================================
class Time final {
	Time() {}
	~Time() {}
public:
	/// ===================================================
	/// public : static methods
	/// ===================================================

	static Time* GetInstance() {
		return &sInstance_;
	}

	static float DeltaTime() {
		return sInstance_.deltaTime_;
	}


	/// ===================================================
	/// public : methods
	/// ===================================================

	void Update();

	void ImGuiDebug();

private:
	/// ===================================================
	/// private : static objects
	/// ===================================================

	static Time sInstance_;
	
	
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::chrono::high_resolution_clock::time_point time_{};
	float deltaTime_ = 0.0f;

	std::deque<float> exeTimes_;
};