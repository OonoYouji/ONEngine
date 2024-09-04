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

		void Update();

		void ImGuiDebug();

		static float DeltaTime() {
			return GetInstance()->deltaTime_;
		}

	private:
		/// ===================================================
		/// private : objects
		/// ===================================================

		std::chrono::high_resolution_clock::time_point start_;
		std::chrono::high_resolution_clock::time_point time_;

		float deltaTime_ = 0.0f;

	private:
		FrameTimer(const FrameTimer&) = delete;
		FrameTimer(FrameTimer&&) = delete;
	};

}
