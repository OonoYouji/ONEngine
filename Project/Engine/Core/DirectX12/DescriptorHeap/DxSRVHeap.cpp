#include "DxSRVHeap.h"

using namespace ONEngine;

/// engine
#include "Engine/Core/Utility/Utility.h"

/// _bufferHeapSize + (_textureHeapSize * 2) 
/// Texture用のSRVとUAVがそれぞれ必要になるため、textureHeapSizeは2倍する

DxSRVHeap::DxSRVHeap(DxDevice* _dxDevice, uint32_t _bufferHeapSize, uint32_t _textureHeapSize) : 
	IDxDescriptorHeap(_dxDevice, _bufferHeapSize + (_textureHeapSize * 2)) {
	/// SRV Texture用のHeapData初期化
	srvTextureHeapData_.usedIndex = 0;
	srvTextureHeapData_.startIndex = 0;
	srvTextureHeapData_.heapSize = _textureHeapSize;

	/// UAV Texture用のHeapData初期化
	uavTextureHeapData_.usedIndex = 0;
	uavTextureHeapData_.startIndex = _textureHeapSize;
	uavTextureHeapData_.heapSize = _textureHeapSize;

	/// Buffer用のHeapData初期化
	bufferHeapData_.usedIndex = 0;
	bufferHeapData_.startIndex = _textureHeapSize * 2;
	bufferHeapData_.heapSize = _bufferHeapSize;
}

DxSRVHeap::~DxSRVHeap() = default;

/**
 * @brief CBV/SRV/UAV用ディスクリプタヒープの生成と管理を担当するクラス
 */
void DxSRVHeap::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxHeapSize_, true);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	Console::Log("dx descriptor heap cbv,srv,uav create success!!");
}


/**
 * @brief 静的テクスチャSRV用の空きディスクリプタ領域を割り当て、その全体ヒープ内インデックスを返します。
 */
uint32_t DxSRVHeap::AllocateTexture() {

	///< 再利用可能なIndexがある場合
	if (srvTextureHeapData_.spaceIndex.size() > 0) {
		uint32_t index = srvTextureHeapData_.spaceIndex.front();
		srvTextureHeapData_.spaceIndex.pop_front();
		return index;
	}

	/// Heapがいっぱいの場合
	if (srvTextureHeapData_.usedIndex >= srvTextureHeapData_.heapSize) {
		Assert(false, "DxSRVHeap::AllocateTexture: Texture SRV Heap is full");
		return UINT32_MAX;
	}


	uint32_t index = srvTextureHeapData_.usedIndex + srvTextureHeapData_.startIndex;
	++srvTextureHeapData_.usedIndex;

	return index;
}

/**
 * @brief UAVテクスチャ（コンピュート書込み用など）用の空きディスクリプタ領域を割り当て、その全体ヒープ内インデックスを返します。
 */
uint32_t DxSRVHeap::AllocateUAVTexture() {

	///< 再利用可能なIndexがある場合
	if (uavTextureHeapData_.spaceIndex.size() > 0) {
		uint32_t index = uavTextureHeapData_.spaceIndex.front();
		uavTextureHeapData_.spaceIndex.pop_front();
		return index;
	}

	/// Heapがいっぱいの場合
	if (uavTextureHeapData_.usedIndex >= uavTextureHeapData_.heapSize) {
		Assert(false, "DxSRVHeap::AllocateUAVTexture: Texture UAV Heap is full");
		return UINT32_MAX;
	}

	uint32_t index = uavTextureHeapData_.usedIndex + uavTextureHeapData_.startIndex;
	++uavTextureHeapData_.usedIndex;

	return index;
}

/**
 * @brief 定数バッファや構造化バッファなどのBuffer SRV/CBV用のディスクリプタ領域を割り当て、そのインデックスを返します。
 */
uint32_t DxSRVHeap::AllocateBuffer() {
	
	///< 再利用可能なIndexがある場合
	if (bufferHeapData_.spaceIndex.size() > 0) {
		uint32_t index = bufferHeapData_.spaceIndex.front();
		bufferHeapData_.spaceIndex.pop_front();
		return index;
	}

	/// Heapがいっぱいの場合
	if (bufferHeapData_.usedIndex >= bufferHeapData_.heapSize) {
		Assert(false, "DxSRVHeap::AllocateBuffer: Buffer Heap is full");
		return UINT32_MAX;
	}

	uint32_t index = bufferHeapData_.usedIndex + bufferHeapData_.startIndex;
	++bufferHeapData_.usedIndex;

	return index;
}

/**
 * @brief テクスチャSRV領域の先頭GPUディスクリプタハンドルを取得します。
 */
D3D12_GPU_DESCRIPTOR_HANDLE DxSRVHeap::GetSRVStartGPUHandle() const {
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (descriptorSize_ * srvTextureHeapData_.startIndex);
	return gpuHandle;
}

/**
 * @brief テクスチャSRV領域の先頭CPUディスクリプタハンドルを取得します。
 */
D3D12_CPU_DESCRIPTOR_HANDLE DxSRVHeap::GetSRVStartCPUHandle() const {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (descriptorSize_ * srvTextureHeapData_.startIndex);
	return cpuHandle;
}

/**
 * @brief UAVテクスチャ領域の先頭GPUディスクリプタハンドルを取得します。
 */
D3D12_GPU_DESCRIPTOR_HANDLE DxSRVHeap::GetUAVStartGPUHandle() const {
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (descriptorSize_ * uavTextureHeapData_.startIndex);
	return gpuHandle;
}

/**
 * @brief UAVテクスチャ領域の先頭CPUディスクリプタハンドルを取得します。
 */
D3D12_CPU_DESCRIPTOR_HANDLE DxSRVHeap::GetUAVStartCPUHandle() const {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (descriptorSize_ * uavTextureHeapData_.startIndex);
	return cpuHandle;
}

/**
 * @brief バッファビュー（CBV/SRV/UAV）領域の先頭GPUディスクリプタハンドルを取得します。
 */
D3D12_GPU_DESCRIPTOR_HANDLE DxSRVHeap::GetBufferStartGPUHandle() const {
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (descriptorSize_ * bufferHeapData_.startIndex);
	return gpuHandle;
}

/**
 * @brief バッファビュー領域の先頭CPUディスクリプタハンドルを取得します。
 */
D3D12_CPU_DESCRIPTOR_HANDLE DxSRVHeap::GetBufferStartCPUHandle() const {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (descriptorSize_ * bufferHeapData_.startIndex);
	return cpuHandle;
}
