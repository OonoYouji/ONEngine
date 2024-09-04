#pragma once

#include <chrono>

namespace ONE {

	/// ===================================================
	/// 実行速度を計算するクラス
	/// ===================================================
	class FrameTimer final {
		FrameTimer() = default;
		~FrameTimer() = default;
	public:

		/// ===================================================
		/// public : methods
		/// ===================================================

		static FrameTimer* GetInstance();

		void Begin();
		float End();

	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		std::chrono::high_resolution_clock::time_point start_;

	private:
		FrameTimer(const FrameTimer&) = delete;
		FrameTimer(FrameTimer&&) = delete;
	};

}
