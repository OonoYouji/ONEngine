#pragma once

#include <chrono>


class WorldTime final {
	WorldTime() {}
	~WorldTime() {}
public:

	static WorldTime* GetInstance() {
		return &sInstance_;
	}

	static float DeltaTime() {
		return sInstance_.deltaTime_;
	}


	void Update();

	void ImGuiDebug();

private:

	static WorldTime sInstance_;

	std::chrono::high_resolution_clock::time_point time_{};
	float deltaTime_ = 0.0f;

};