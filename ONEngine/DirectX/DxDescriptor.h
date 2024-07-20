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


		/// <summary>
		/// RTVのCPUHandleを得る
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle();

		/// <summary>
		/// RTVの使用カウントを増やす
		/// </summary>
		void AddSrvUsedCount();

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

		/// <summary>
		/// 引数のHeapからindex分先のGpuHandleを得る
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(ID3D12DescriptorHeap* heap, uint32_t heapSize, uint32_t index);

	private:

		/// ===================================================
		/// private : objects
		/// ===================================================

		ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
		uint32_t rtvUsedCount_ = 0;
		uint32_t rtvHeapSize_;

		ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;
		uint32_t srvUsedCount_ = 0;
		uint32_t srvHeapSize_;


	private:
		DxDescriptor(const DxDescriptor&) = delete;
		DxDescriptor(DxDescriptor&&) = delete;
		DxDescriptor& operator= (const DxDescriptor&) = delete;
		DxDescriptor& operator= (DxDescriptor&&) = delete;
	};

}