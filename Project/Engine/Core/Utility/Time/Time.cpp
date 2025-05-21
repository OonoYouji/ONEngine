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

		TimeController() {}
		~TimeController() {}
		
		void Update();


	private:

		/// =======================================================
		/// private : objects
		/// =======================================================

		float                                          deltaTime_;
		std::chrono::high_resolution_clock::time_point time_;
	};


	/// =======================================================
	/// public : methods
	/// =======================================================

	void TimeController::Update() {
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> duration = end - time_;
		time_ = std::chrono::high_resolution_clock::now();

		deltaTime_ = duration.count() / 1000.0f;  ///< 秒に変換
		deltaTime_ = std::min(deltaTime_, 1.0f / 60.0f); ///< 0.05秒以上は無視
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

float Time::DeltaTime() {
	return gTimeController->deltaTime_;
}

