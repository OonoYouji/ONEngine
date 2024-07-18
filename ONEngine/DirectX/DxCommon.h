#pragma once

#include <memory>

namespace ONE {

	class DxDevice;
	class DxCommand;
	class DxDoubleBuffer;
	class DxDescriptor;
	class DxDebug;
	class DxShaderCompiler;

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

		/// <summary>
		/// インスタンス確保
		/// </summary>
		static DxCommon* GetInstance();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		void PreDraw();

		void PostDraw();

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================


	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		std::unique_ptr<DxDevice> device_ = nullptr;
		std::unique_ptr<DxCommand> command_ = nullptr;
		std::unique_ptr<DxDescriptor> descriptor_ = nullptr;
		std::unique_ptr<DxDoubleBuffer> doubleBuffer_ = nullptr;

		std::unique_ptr<DxDebug> debug_ = nullptr;

		std::unique_ptr<DxShaderCompiler> shaderCompiler_ = nullptr;

	private:
		DxCommon(const DxCommon&) = delete;
		DxCommon(DxCommon&&) = delete;
		DxCommon& operator=(const DxCommon&) = delete;
		DxCommon& operator=(DxCommon&&) = delete;
	};

}