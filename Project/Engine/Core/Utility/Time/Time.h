#pragma once


/// /////////////////////////////////////////////////////////////
/// Time
/// /////////////////////////////////////////////////////////////
namespace ONEngine {

class Time {
	friend class GameFramework;
private:
	Time();
	~Time() ;

	static void Initialize();
	static void Finalize();
	static void Update();

public:
	/// =======================================================
	/// public : methods
	/// =======================================================

	/// @brief 時間をリセットする
	static void ResetTime();

	/// @brief 経過時間を取得する
	static float GetTime();

	/// @brief デルタタイムを取得する
	static float DeltaTime();

	/// @brief スケールを考慮しないデルタタイムを取得する
	static float UnscaledDeltaTime();

	/// @brief 時間のスケールを取得・設定する
	static float TimeScale();

	/// @brief TimeScaleを設定する
	/// @param _timeScale scale値
	static void SetTimeScale(float _timeScale);
};



} /// ONEngine
