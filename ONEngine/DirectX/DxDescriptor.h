#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cmath>

using namespace Microsoft::WRL;



namespace ONE {

	/// ===================================================
	/// DirectX12 Descriptorの管理クラス
	/// ===================================================
	class DxDescriptor final {
	public:

		DxDescriptor();
		~DxDescriptor();

		/// ===================================================
		/// public : methods
		/// ===================================================

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// RTVのCPUHandleを得る
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle();

		/// <summary>
		/// RTVの使用カウントを増やす
		/// </summary>
		void AddRtvUsedCount();

	private:

		/// ===================================================
		/// private : methods
		/// ===================================================

		/// <summary>
		/// DescriptorHeapの生成関数
		/// </summary>
		ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

		/// <summary>
		/// 引数のHeapからindex分先のCpuHandleを得る
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(ID3D12DescriptorHeap* heap, uint32_t heapSize, uint32_t index);

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;

		uint32_t rtvUsedCount_ = 0;
		uint32_t rtvHeapSize_;

	private:
		DxDescriptor(const DxDescriptor&) = delete;
		DxDescriptor(DxDescriptor&&) = delete;
		DxDescriptor& operator= (const DxDescriptor&) = delete;
		DxDescriptor& operator= (DxDescriptor&&) = delete;
	};

}