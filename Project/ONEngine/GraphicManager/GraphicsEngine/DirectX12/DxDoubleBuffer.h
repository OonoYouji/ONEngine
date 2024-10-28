#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <vector>

#include "DxDescriptorHeap.h"

using namespace Microsoft::WRL;



namespace ONE {

	class WinApp;
	class DxDevice;
	class DxCommand;

	/// ===================================================
	/// DirectX12 ダブルバッファ
	/// ===================================================
	class DxDoubleBuffer final {
	public:
		/// ===================================================
		/// public : static objects
		/// ===================================================

		static const int kBufferCount = 2;

	public:


		DxDoubleBuffer();
		~DxDoubleBuffer();

		/// ===================================================
		/// public : methods
		/// ===================================================

		/// <summary>
		/// このクラスの初期化
		/// </summary>
		void Initialize(
			WinApp* winApp,
			DxDevice* dxDevice,
			DxDescriptorHeap<HeapType::RTV>* _rtvDescriptorHeap, 
			DxDescriptorHeap<HeapType::DSV>* _dsvDescriptorHeap, 
			ID3D12CommandQueue* commandQueue
		);

		void ClearBB(ID3D12GraphicsCommandList* commandList);

		void Present();

		void CreateBarrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		
		void SetViewport(ID3D12GraphicsCommandList* commandList);
		void SetSiccorRect(ID3D12GraphicsCommandList* commandList);

		void SetRanderTarget(DxCommand* command);

		void CopyToBB(ID3D12GraphicsCommandList* commnadList, ID3D12Resource* resource, D3D12_RESOURCE_STATES current);

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================

		/// <summary>
		/// SwapChainの初期化
		/// </summary>
		void InitializeSwapChain(IDXGIFactory7* factory,ID3D12CommandQueue* commandQueue);

		void InitializeBuffers(ID3D12Device* device, DxDescriptorHeap<HeapType::RTV>* _rtvDescriptorHeap);

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
		std::vector<ComPtr<ID3D12Resource>> buffers_{};
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandle_;

		D3D12_VIEWPORT viewprot_{};
		D3D12_RECT sicssorRect_{};

		WinApp* pWinApp_ = nullptr;
		DxDescriptorHeap<HeapType::DSV>* pDSVDescriptorHeap_ = nullptr;
		uint32_t dsvDescriptorIndex_;

	private:
		DxDoubleBuffer(const DxDoubleBuffer&) = delete;
		DxDoubleBuffer(DxDoubleBuffer&&) = delete;
		DxDoubleBuffer& operator=(const DxDoubleBuffer&) = delete;
		DxDoubleBuffer& operator=(DxDoubleBuffer&&) = delete;
	};


}