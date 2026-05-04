#pragma once

namespace Engine::Core {

class Timer {
public:
    Timer();

    void Reset();
    void Tick();

    float GetDeltaTime() const { return static_cast<float>(deltaTime_); }
    double GetTotalTime() const { return totalTime_; }

private:
    double secondsPerCount_ = 0.0;
    double deltaTime_ = -1.0;

    long long baseTime_ = 0;
    long long pausedTime_ = 0;
    long long stopTime_ = 0;
    long long prevTime_ = 0;
    long long currTime_ = 0;
    double totalTime_ = 0.0;

    bool isStopped_ = false;
};

} // namespace Engine::Core
