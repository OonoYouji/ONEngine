#pragma once

#include <cmath>
#include <chrono>



/// ===================================================
/// FPSを固定するクラス
/// ===================================================
class FrameFixation final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	FrameFixation() {}
	~FrameFixation() {}

	void Initialize(bool isFixation, uint32_t maxFrameRate);

	void ImGuiDebug();

	/// <summary>
	/// 固定化
	/// </summary>
	void Fixation();

private:
	/// ===================================================
	/// private : const objects
	/// ===================================================

	uint32_t  maxFrameRate_;	/// 最大FPS
	long long minEspsetTime_;


	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isFixation_ = true;	/// 固定化フラグ

	std::chrono::steady_clock::time_point currentTime_; /// 今の時間

};