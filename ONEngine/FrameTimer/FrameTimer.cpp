#include <FrameTimer.h>

#include <ImGuiManager.h>


/// ===================================================
/// 初期化
/// ===================================================
ONE::FrameTimer* ONE::FrameTimer::GetInstance() {
	static ONE::FrameTimer instance;
	return &instance;
}


/// ===================================================
/// スタート
/// ===================================================
void ONE::FrameTimer::Begin() {
	start_ = std::chrono::high_resolution_clock::now();
}



/// ===================================================
/// エンド
/// ===================================================
float ONE::FrameTimer::End() {
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = end - start_;

	///- 秒に変換
	return duration.count() / 1000.0f;
}
