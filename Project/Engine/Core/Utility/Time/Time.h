#pragma once


/// /////////////////////////////////////////////////////////////
/// Time
/// /////////////////////////////////////////////////////////////
class Time {
	friend class GameFramework;
private:

	Time() {}
	~Time() {}

	static void Initialize();
	static void Finalize();
	static void Update();

public:

	/// =======================================================
	/// public : methods
	/// =======================================================

	static float GetTime();
	static float DeltaTime();
	static float UnscaledDeltaTime();

	static float TimeScale();
	static void SetTimeScale(float _timeScale);
};


