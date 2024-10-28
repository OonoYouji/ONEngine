#pragma once

/// directX12
#include <d3d12.h>

/// std
#include <memory>

/// engine
#include "DxDescriptorHeap.h"

class RenderTexture;

namespace ONE {

	class WinApp;
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
	public:
		DxCommon() = default;
		~DxCommon() = default;

		/// ===================================================
		/// public : methods
		/// ===================================================

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		void CommandExecution();

		/// <summary>
		/// CommandList内にある命令を実行する
		/// </summary>
		void ExecuteCommands();


		void SetRenderTarget();


	public:

		/// ===================================================
		/// public : accessor
		/// ===================================================

		DxShaderCompiler* GetDxShaderCompiler() const { return shaderCompiler_.get(); }

		DxCommand* GetDxCommand() const { return command_.get(); }

		DxDevice*     GetDxDevice() const { return device_.get(); }
		ID3D12Device* GetDevice()   const;

		//DxDescriptor* GetDxDescriptor() const { return descriptor_.get(); }

		DxDescriptorHeap<HeapType::CBV_SRV_UAV>* GetSRVDescriptorHeap() const { return srvDescriptorHeap_.get(); }
		DxDescriptorHeap<HeapType::RTV>*         GetRTVDescriptorHeap() const { return rtvDescriptorHeap_.get(); }
		DxDescriptorHeap<HeapType::DSV>*         GetDSVDescriptorHeap() const { return dsvDescriptorHeap_.get(); }

		DxDepthStencil* GetDxDepthStencil() const { return depthStencil.get(); }

		bool IsGpuExeEnded();

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		std::unique_ptr<DxDevice>       device_       = nullptr;
		std::unique_ptr<DxCommand>      command_      = nullptr;
		std::unique_ptr<DxDescriptor>   descriptor_   = nullptr;
		std::unique_ptr<DxDepthStencil> depthStencil  = nullptr;


		std::unique_ptr<DxDescriptorHeap<HeapType::CBV_SRV_UAV>> srvDescriptorHeap_ = nullptr;
		std::unique_ptr<DxDescriptorHeap<HeapType::RTV>>         rtvDescriptorHeap_ = nullptr;
		std::unique_ptr<DxDescriptorHeap<HeapType::DSV>>         dsvDescriptorHeap_ = nullptr;

		std::unique_ptr<DxDebug> debug_ = nullptr;

		std::unique_ptr<DxShaderCompiler> shaderCompiler_ = nullptr;

	};

}