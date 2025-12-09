#pragma once

/// engine
#include "IDxDescriptorHeap.h"

/// /////////////////////////////////////////////////
/// DescriptorHeapの基底クラス
/// /////////////////////////////////////////////////
namespace ONEngine {

class DxSRVHeap final : public IDxDescriptorHeap {

	struct HeapData {
		uint32_t usedIndex;
		uint32_t startIndex;
		uint32_t heapSize;
		std::deque<uint32_t> spaceIndex;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DxSRVHeap(DxDevice* _dxDevice, uint32_t _bufferHeapSize, uint32_t _textureHeapSize);
	~DxSRVHeap();

	/// @brief 初期化
	void Initialize() override;
	
	/// @brief Texture用のDescriptorHeapのIndexを取得する
	/// @return DescriptorHeapのIndex
	uint32_t AllocateTexture();

	/// @brief UAVTexture用のDescriptorHeapのIndexを取得する
	/// @return DescriptorHeapのIndex
	uint32_t AllocateUAVTexture();

	/// @brief Buffer用のDescriptorHeapのIndexを取得する
	/// @return DescriptorHeapのIndex
	uint32_t AllocateBuffer();


	/// @brief SRV用のDescriptorHeapの開始GPUハンドルを取得する
	/// @return SRV用のDescriptorHeapの開始GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVStartGPUHandle() const;

	/// @brief SRV用のDescriptorHeapの開始CPUハンドルを取得する
	/// @return SRV用のDescriptorHeapの開始CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVStartCPUHandle() const;


	/// @brief UAV用のDescriptorHeapの開始GPUハンドルを取得する
	/// @return UAV用のDescriptorHeapの開始GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVStartGPUHandle() const;

	/// @brief UAV用のDescriptorHeapの開始CPUハンドルを取得する
	/// @return UAV用のDescriptorHeapの開始CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVStartCPUHandle() const;


	/// @brief Buffer用のDescriptorHeapの開始GPUハンドルを取得する
	/// @return Buffer用のDescriptorHeapの開始GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE GetBufferStartGPUHandle() const;

	/// @brief Buffer用のDescriptorHeapの開始CPUハンドルを取得する
	/// @return Buffer用のDescriptorHeapの開始CPUハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE GetBufferStartCPUHandle() const;

	/// @brief 基底クラスのAllocateは使用禁止
	uint32_t Allocate() = delete;

private:
	/// ===================================================`
	/// private : objects
	/// ===================================================`

	HeapData srvTextureHeapData_; /// SRVのTexture用
	HeapData uavTextureHeapData_; /// UAVのTexture用
	HeapData bufferHeapData_;

};

} /// ONEngine
