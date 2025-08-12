#pragma once

/// directX12
#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <cstdint>
#include <span>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DxSRVHeap.h"
#include "Engine/Core/Utility/Tools/Assert.h"


/// ===================================================
/// ストラクチャードバッファ用クラス
/// ===================================================
template <typename T>
class StructuredBuffer final {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	StructuredBuffer();
	~StructuredBuffer();

	void Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap);
	void CreateUAV(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap);
	void CreateAppendBuffer(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap);

	/* ----- append structure buffer methods ----- */

	void ResetCounter(DxCommand* _dxCommand);
	uint32_t ReadCounter(DxCommand* _dxCommand);


	void BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList);
	void BindForComputeCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList);

	void SetMappedData(size_t _index, const T& _setValue);

	DxResource& GetResource();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	uint32_t                    srvDescriptorIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

	DxResource                  bufferResource_;
	T* mappedData_;
	std::span<T>                mappedDataArray_;

	size_t                      structureSize_;
	size_t                      totalSize_;
	size_t                      bufferSize_;

	/// append buffer用
	DxResource                  counterResource_;
	DxResource                  counterResetResource_;
	DxResource                  readbackResource_;
	bool                        isAppendBuffer_ = false;


	DxSRVHeap* pDxSRVHeap_;
};


/// ===================================================
/// public : method definition
/// ===================================================

template<typename T>
inline StructuredBuffer<T>::StructuredBuffer() {
	pDxSRVHeap_ = nullptr;
}

template<typename T>
inline StructuredBuffer<T>::~StructuredBuffer() {
	if (pDxSRVHeap_) {
		pDxSRVHeap_->Free(srvDescriptorIndex_);
	}
}


template<typename T>
inline void StructuredBuffer<T>::Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {

	/// bufferのサイズを計算
	structureSize_ = sizeof(T);
	bufferSize_ = _size;
	totalSize_ = structureSize_ * bufferSize_;

	/// bufferの生成
	bufferResource_.CreateResource(_dxDevice, totalSize_);

	/// desc setting
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements = static_cast<UINT>(bufferSize_);
	desc.Buffer.StructureByteStride = static_cast<UINT>(structureSize_);

	/// cpu, gpu handle initialize
	pDxSRVHeap_ = _dxSRVHeap;
	srvDescriptorIndex_ = pDxSRVHeap_->AllocateBuffer();
	cpuHandle_ = pDxSRVHeap_->GetCPUDescriptorHandel(srvDescriptorIndex_);
	gpuHandle_ = pDxSRVHeap_->GetGPUDescriptorHandel(srvDescriptorIndex_);

	/// resource create
	_dxDevice->GetDevice()->CreateShaderResourceView(bufferResource_.Get(), &desc, cpuHandle_);

	/// mapping
	bufferResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	mappedDataArray_ = { mappedData_, bufferSize_ };
}

template<typename T>
inline void StructuredBuffer<T>::CreateUAV(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {

	/// bufferのサイズを計算
	structureSize_ = sizeof(T);
	bufferSize_ = _size;
	totalSize_ = structureSize_ * bufferSize_;

	/// bufferの生成
	bufferResource_.CreateUAVResource(_dxDevice, _dxCommand, totalSize_);

	/// desc setting
	D3D12_UNORDERED_ACCESS_VIEW_DESC  desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = static_cast<UINT>(bufferSize_);
	desc.Buffer.StructureByteStride = static_cast<UINT>(structureSize_);
	desc.Buffer.CounterOffsetInBytes = 0;
	desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	/// cpu, gpu handle initialize
	pDxSRVHeap_ = _dxSRVHeap;
	srvDescriptorIndex_ = pDxSRVHeap_->AllocateBuffer();
	cpuHandle_ = pDxSRVHeap_->GetCPUDescriptorHandel(srvDescriptorIndex_);
	gpuHandle_ = pDxSRVHeap_->GetGPUDescriptorHandel(srvDescriptorIndex_);

	/// resource create
	_dxDevice->GetDevice()->CreateUnorderedAccessView(bufferResource_.Get(), nullptr, &desc, cpuHandle_);
}

template<typename T>
inline void StructuredBuffer<T>::CreateAppendBuffer(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {
	isAppendBuffer_ = true;

	structureSize_ = sizeof(T);
	bufferSize_ = _size;
	totalSize_ = structureSize_ * bufferSize_;

	bufferResource_.CreateUAVResource(_dxDevice, _dxCommand, totalSize_);

	D3D12_RESOURCE_DESC counterDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	ThrowIfFailed(_dxDevice->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&counterDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(counterResource_.GetAddressOf())
	));

	{	/// カウンタリセット用アップロードバッファを用意(0を1つ)
		D3D12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT));
		ThrowIfFailed(_dxDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&uploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(counterResetResource_.GetAddressOf())
		));

		UINT* mapped = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(counterResetResource_->Map(0, &readRange, reinterpret_cast<void**>(&mapped)));
		*mapped = 0;
		counterResetResource_->Unmap(0, nullptr);
	}

	{	/// カウンタ読み取り用リードバックバッファ(非同期読み込み用)
		D3D12_RESOURCE_DESC readbackDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT));
		ThrowIfFailed(_dxDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
			D3D12_HEAP_FLAG_NONE,
			&readbackDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(readbackResource_.GetAddressOf())
		));
	}

	// UAVビュー作成（カウンタバッファをセット）
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = bufferSize_;
	desc.Buffer.StructureByteStride = static_cast<UINT>(structureSize_);
	desc.Buffer.CounterOffsetInBytes = 0;
	desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	pDxSRVHeap_ = _dxSRVHeap;
	srvDescriptorIndex_ = pDxSRVHeap_->AllocateBuffer();
	cpuHandle_ = pDxSRVHeap_->GetCPUDescriptorHandel(srvDescriptorIndex_);
	gpuHandle_ = pDxSRVHeap_->GetGPUDescriptorHandel(srvDescriptorIndex_);

	_dxDevice->GetDevice()->CreateUnorderedAccessView(bufferResource_.Get(), counterResource_.Get(), &desc, cpuHandle_);
}

template<typename T>
inline void StructuredBuffer<T>::ResetCounter(DxCommand* _dxCommand) {
	auto cmdList = _dxCommand->GetCommandList();
	Assert(isAppendBuffer_, "ResetCounter is only valid for AppendBuffer");

	/// UploadHeapからDefaultHeapのCounterリソースに0をコピー
	cmdList->CopyBufferRegion(counterResource_.Get(), 0, counterResetResource_.Get(), 0, sizeof(UINT));

	/// UAV Barrier で確実にリセットを保証
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(counterResource_.Get());
	cmdList->ResourceBarrier(1, &barrier);
}

template<typename T>
inline uint32_t StructuredBuffer<T>::ReadCounter(DxCommand* _dxCommand) {
	Assert(isAppendBuffer_, "ReadCounter is only valid for AppendBuffer");

	auto cmdList = _dxCommand->GetCommandList();

	/// DefaultHeapのCounterリソースからReadbackHeapにコピー
	cmdList->CopyResource(readbackResource_.Get(), counterResource_.Get());

	/// コマンドリストを閉じて実行し、フェンス待ちなどが必要（ここでは省略）
	_dxCommand->CommandExecute();
	_dxCommand->CommandReset();

	/// CPUでマップして読み取り
	uint32_t* mapped = nullptr;
	CD3DX12_RANGE readRange(0, sizeof(UINT));
	ThrowIfFailed(readbackResource_->Map(0, &readRange, reinterpret_cast<void**>(&mapped)));
	uint32_t count = *mapped;
	readbackResource_->Unmap(0, nullptr);

	return count;
}

template<typename T>
inline void StructuredBuffer<T>::BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	_commandList->SetGraphicsRootDescriptorTable(_rootParameterIndex, gpuHandle_);
}

template<typename T>
inline void StructuredBuffer<T>::BindForComputeCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	_commandList->SetComputeRootDescriptorTable(_rootParameterIndex, gpuHandle_);
}

template<typename T>
inline void StructuredBuffer<T>::SetMappedData(size_t _index, const T& _setValue) {
	Assert(_index < bufferSize_, "out of range");
	mappedDataArray_[_index] = _setValue;
}

template<typename T>
inline DxResource& StructuredBuffer<T>::GetResource() {
	return bufferResource_;
}
