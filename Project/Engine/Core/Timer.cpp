#include "Timer.h"
#include <Windows.h>

namespace Engine::Core {

Timer::Timer() {
    long long countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    secondsPerCount_ = 1.0 / (double)countsPerSec;
}

void Timer::Reset() {
    long long currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

    baseTime_ = currTime;
    prevTime_ = currTime;
    stopTime_ = 0;
    isStopped_ = false;
}

void Timer::Tick() {
    if (isStopped_) {
        deltaTime_ = 0.0;
        return;
    }

    long long currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    currTime_ = currTime;

    deltaTime_ = (currTime_ - prevTime_) * secondsPerCount_;

    prevTime_ = currTime_;

    if (deltaTime_ < 0.0) {
        deltaTime_ = 0.0;
    }

    totalTime_ = (currTime_ - pausedTime_ - baseTime_) * secondsPerCount_;
}

} // namespace Engine::Core
