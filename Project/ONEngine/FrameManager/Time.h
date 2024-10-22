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

	static float TimeRateDeltaTime() {
		return sInstance_.deltaTime_ * sInstance_.timeRate_;
	}

	/// <summary>
	/// 時間の流れる速さをセット
	/// 0 → 全く動かない
	/// 1 → 正常の動き
	/// 2 → 倍速の動き
	/// </summary>
	/// <param name="_timeRate"></param>
	static void SetTimeRate(float _timeRate);


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
	float timeRate_  = 1.0f;

	std::deque<float> exeTimes_;
};