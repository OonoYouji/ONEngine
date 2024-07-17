#pragma once

#include <memory>

class DxDevice;

namespace ONE {

	/// ===================================================
	/// DirectX12汎用クラス
	/// ===================================================
	class DxCommon final {
		DxCommon() = default;
		~DxCommon() = default;
	public:

		/// ===================================================
		/// public : methods
		/// ===================================================

		static DxCommon* GetInstance();

		void Initialize();

		void Finalize();

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================


	private:
		
		/// ===================================================
		/// private : objects
		/// ===================================================

		std::unique_ptr<DxDevice> device_ = nullptr;


	private:
		DxCommon(const DxCommon&) = delete;
		DxCommon(DxCommon&&) = delete;
		DxCommon& operator=(const DxCommon&) = delete;
		DxCommon& operator=(DxCommon&&) = delete;
	};

}