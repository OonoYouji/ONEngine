#include "Time.h"

/// std
#include <chrono>
#include <memory>


/// /////////////////////////////////////////////////////////////
/// 無記名namespace
/// /////////////////////////////////////////////////////////////
namespace {

	/// /////////////////////////////////////////////////////////////
	/// TimeController
	/// /////////////////////////////////////////////////////////////
	class TimeController {
		friend class Time;
	public:

		/// =======================================================
		/// public : methods
		/// =======================================================

		TimeController() {
			timeScale_ = 1.0f;
		}
		~TimeController() {}

		void Update();


	private:

		/// =======================================================
		/// private : objects
		/// =======================================================

		float deltaTime_;
		float unscaledDeltaTime_;
		float timeScale_;
		float gameTime_ = 0.0f;

		std::chrono::high_resolution_clock::time_point time_;
	};


	/// =======================================================
	/// public : methods
	/// =======================================================

	void TimeController::Update() {
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> duration = end - time_;
		time_ = std::chrono::high_resolution_clock::now();

		unscaledDeltaTime_ = duration.count() / 1000.0f;  ///< 秒に変換
		deltaTime_ = unscaledDeltaTime_ * timeScale_;  ///< 時間のスケールを適用

		gameTime_ += deltaTime_;
	}


	/// =======================================================
	/// instance
	/// =======================================================

	std::unique_ptr<TimeController> gTimeController;

} /// namespace



void Time::Initialize() {
	gTimeController = std::make_unique<TimeController>();
}

void Time::Finalize() {
	gTimeController.reset();
}

void Time::Update() {
	gTimeController->Update();
}

void Time::ResetTime() {
	gTimeController->gameTime_ = 0.0f;
}

float Time::GetTime() {
	return gTimeController->gameTime_;
}

float Time::DeltaTime() {
	return gTimeController->deltaTime_;
}

float Time::UnscaledDeltaTime() {
	return gTimeController->unscaledDeltaTime_;
}

float Time::TimeScale() {
	return gTimeController->timeScale_;
}

void Time::SetTimeScale(float _timeScale) {
	gTimeController->timeScale_ = _timeScale;
}

