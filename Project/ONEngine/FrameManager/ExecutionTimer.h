#pragma once

/// std
#include <chrono>

enum TIME_UNIT {
	TIME_UNIT_MICROSECONDS, // マイクロ秒
	TIME_UNIT_MILLSECONDS,  // ミリ秒
	TIME_UNIT_SECONDS,      // 秒
	TIME_UNIT_COUNT
};


class ExecutionTimer final {
public:

	ExecutionTimer() { Initialize(); }
	~ExecutionTimer() {}

	void Initialize();

	void StartTimer();
	float EndTimer(TIME_UNIT _timeUnit);

private:
	std::chrono::steady_clock::time_point startTime_;
};
