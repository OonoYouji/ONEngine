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

	void Initialize(bool isFixation);

	void ImGuiDebug();

	/// <summary>
	/// 固定化
	/// </summary>
	void Fixation();

private:
	/// ===================================================
	/// private : const objects
	/// ===================================================

	const uint32_t  kFrameNumber_   = 60u;	/// 最大FPS
	const long long kMinEspsetTime_ = 1000000ll / static_cast<long long>(kFrameNumber_);


	/// ===================================================
	/// private : objects
	/// ===================================================

	bool isFixation_ = true;	/// 固定化フラグ

	std::chrono::steady_clock::time_point currentTime_; /// 今の時間

};