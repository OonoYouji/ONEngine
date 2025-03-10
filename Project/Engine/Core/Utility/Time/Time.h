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

	static float DeltaTime();

};


