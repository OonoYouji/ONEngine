#pragma once

#include <d3d12.h>

#include <memory>

namespace ONE {

	class DxDevice;
	class DxCommand;
	class DxDoubleBuffer;
	class DxDescriptor;
	class DxDebug;
	class DxShaderCompiler;
	class DxDepthStencil;

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

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画後処理
		/// </summary>
		void PostDraw();

		/// <summary>
		/// CommandList内にある命令を実行する
		/// </summary>
		void ExecuteCommands();


	public:

		/// ===================================================
		/// public : accessor
		/// ===================================================

		DxShaderCompiler* GetDxShaderCompiler() const { return shaderCompiler_.get(); }

		DxCommand* GetDxCommand() const { return command_.get(); }

		ID3D12Device* GetDevice() const;

		DxDescriptor* GetDxDescriptor() const { return descriptor_.get(); }

		DxDepthStencil* GetDxDepthStencil() const { return depthStencil.get(); }

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		std::unique_ptr<DxDevice> device_ = nullptr;
		std::unique_ptr<DxCommand> command_ = nullptr;
		std::unique_ptr<DxDescriptor> descriptor_ = nullptr;
		std::unique_ptr<DxDoubleBuffer> doubleBuffer_ = nullptr;
		std::unique_ptr<DxDepthStencil> depthStencil = nullptr;

		std::unique_ptr<DxDebug> debug_ = nullptr;

		std::unique_ptr<DxShaderCompiler> shaderCompiler_ = nullptr;

	private:
		DxCommon(const DxCommon&) = delete;
		DxCommon(DxCommon&&) = delete;
		DxCommon& operator=(const DxCommon&) = delete;
		DxCommon& operator=(DxCommon&&) = delete;
	};

}