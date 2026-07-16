#include "Time.h"

using namespace ONEngine;

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
			time_ = std::chrono::high_resolution_clock::now();
			deltaTime_ = 0.0f;
			unscaledDeltaTime_ = 0.0f;
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

	/**
	 * @brief 時間計測の更新処理。前フレームからの経過時間を計算し、累計時間を進めます。
	 */
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


Time::Time() = default;
Time::~Time() = default;

/**
	 * @brief 時間計測システムの初期化を行います。
	 */
	void Time::Initialize() {
	gTimeController = std::make_unique<TimeController>();
}

/**
	 * @brief 時間計測システムの終了処理を行います。
	 */
	void Time::Finalize() {
	gTimeController.reset();
}

/**
	 * @brief 毎フレーム呼び出され、経過時間とデルタタイムを更新します。
	 */
	void Time::Update() {
	gTimeController->Update();
}

/**
	 * @brief 蓄積されたゲーム経過時間をリセットします。
	 */
	void Time::ResetTime() {
	gTimeController->gameTime_ = 0.0f;
}

/**
	 * @brief ゲーム開始からの累計経過時間（秒）を取得します。
	 * @return 累計経過時間（秒）
	 */
	float Time::GetTime() {
	return gTimeController->gameTime_;
}

/**
	 * @brief 前フレームからの経過時間（秒）を取得します。
	 * @return デルタタイム（秒）
	 */
	float Time::DeltaTime() {
	return gTimeController->deltaTime_;
}

/**
	 * @brief タイムスケールによる影響を受けない、実際のフレーム間経過時間（秒）を取得します。
	 * @return 補正前のデルタタイム（秒）
	 */
	float Time::UnscaledDeltaTime() {
	return gTimeController->unscaledDeltaTime_;
}

/**
	 * @brief 現在の時間進行スケール率を取得します。
	 * @return タイムスケール値
	 */
	float Time::TimeScale() {
	return gTimeController->timeScale_;
}

/**
	 * @brief 時間進行スケール率を設定します。
	 * @param _timeScale 設定するタイムスケール倍率値
	 */
	void Time::SetTimeScale(float _timeScale) {
	gTimeController->timeScale_ = _timeScale;
}

