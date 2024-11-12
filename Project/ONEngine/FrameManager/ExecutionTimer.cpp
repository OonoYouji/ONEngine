#include "ExecutionTimer.h"

/// std
#include <array>

/// lib
#include "Debugger/Assertion.h"


namespace {

	std::array<float, TIME_UNIT_COUNT> conversionFactor;

} /// namespace


void ExecutionTimer::Initialize() {
	for(size_t i = 0; i < TIME_UNIT_COUNT; ++i) {
		conversionFactor[i] = static_cast<float>(std::pow(10.0f, i + 1));
	}

	StartTimer();
}

void ExecutionTimer::StartTimer() {
	startTime_ = std::chrono::high_resolution_clock::now();
}

float ExecutionTimer::EndTimer(TIME_UNIT _timeUnit) {
	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();

	/// 差分を取って実行時間を測定
	std::chrono::duration<float, std::milli> diff = end - startTime_;
	float result = diff.count() / conversionFactor[_timeUnit];

	return result;
}

