#pragma once

/// directX12
#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <cstdint>
#include <span>
#include <optional>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Engine/Core/DirectX12/Command/DxCommand.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DxSRVHeap.h"
#include "Engine/Core/Utility/Tools/Assert.h"

struct Handle {
	uint32_t heapIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};


/// ///////////////////////////////////////////////////
/// ストラクチャードバッファ用クラス
/// ///////////////////////////////////////////////////
template <typename T>
class StructuredBuffer final {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	StructuredBuffer();
	~StructuredBuffer();

	/// @brief SRVバッファの生成
	/// @param _size Bufferのサイズ
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	void Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap);

	/// @brief UAVバッファの生成
	/// @param _size Bufferのサイズ
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	void CreateUAV(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap);

	/// @brief AppendBufferの生成
	/// @param _size Bufferのサイズ
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	void CreateAppendBuffer(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap);

	/// @brief SRVとUAVの両方を生成する
	/// @param _size Bufferのサイズ
	/// @param _dxDevice DxDeviceへのポインタ
	/// @param _dxCommand DxCommandへのポインタ
	/// @param _dxSRVHeap DxSRVHeapへのポインタ
	void CreateSRVAndUAV(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap);



	/* ----- append structure buffer methods ----- */

	/// @brief AppendBufferのカウンタをリセットする
	/// @param _dxCommand DxCommandへのポインタ
	void ResetCounter(DxCommand* _dxCommand);

	/// @brief AppendBufferのカウンタを読み取る
	/// @param _dxCommand DxCommandへのポインタ
	/// @return Counterの値
	uint32_t ReadCounter(DxCommand* _dxCommand);


	/// SRV用のバインド
	void SRVBindForGraphicsCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const;
	void SRVBindForComputeCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const;

	/// UAV用のバインド
	void UAVBindForGraphicsCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const;
	void UAVBindForComputeCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const;

	/// Append用のバインド
	void AppendBindForGraphicsCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const;
	void AppendBindForComputeCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const;


	/// データの設定、取得
	void SetMappedData(size_t _index, const T& _setValue);
	const T& GetMappedData(size_t _index) const;


	/// 各種Resourceの取得
	const DxResource& GetResource() const;
	const DxResource& GetCounterResource() const;
	const DxResource& GetCounterResetResource() const;
	const DxResource& GetReadbackResource() const;

	DxResource& GetResource();
	DxResource& GetCounterResource();
	DxResource& GetCounterResetResource();
	DxResource& GetReadbackResource();

private:

	/// ===================================================
	/// private : objects
	/// ===================================================

	std::optional<Handle> srvHandle_;
	std::optional<Handle> uavHandle_;
	std::optional<Handle> appendHandle_;

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

		if (srvHandle_.has_value()) {
			pDxSRVHeap_->Free(srvHandle_->heapIndex);
		}

		if (uavHandle_.has_value()) {
			pDxSRVHeap_->Free(uavHandle_->heapIndex);
		}

		if (appendHandle_.has_value()) {
			pDxSRVHeap_->Free(appendHandle_->heapIndex);
		}
	}
}


template<typename T>
inline void StructuredBuffer<T>::Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {

	/// 生成済みならこのhandleを解放する
	if (srvHandle_.has_value()) {
		_dxSRVHeap->Free(srvHandle_->heapIndex);
	}

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
	srvHandle_ = std::make_optional<Handle>();
	srvHandle_->heapIndex = pDxSRVHeap_->AllocateBuffer();
	srvHandle_->cpuHandle = pDxSRVHeap_->GetCPUDescriptorHandel(srvHandle_->heapIndex);
	srvHandle_->gpuHandle = pDxSRVHeap_->GetGPUDescriptorHandel(srvHandle_->heapIndex);

	/// resource create
	_dxDevice->GetDevice()->CreateShaderResourceView(bufferResource_.Get(), &desc, srvHandle_->cpuHandle);

	/// mapping
	bufferResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	mappedDataArray_ = { mappedData_, bufferSize_ };
}

template<typename T>
inline void StructuredBuffer<T>::CreateUAV(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {

	/// 生成済みならこのhandleを解放する
	if (uavHandle_.has_value()) {
		_dxSRVHeap->Free(uavHandle_->heapIndex);
	}


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
	uavHandle_ = std::make_optional<Handle>();
	uavHandle_->heapIndex = pDxSRVHeap_->AllocateBuffer();
	uavHandle_->cpuHandle = pDxSRVHeap_->GetCPUDescriptorHandel(uavHandle_->heapIndex);
	uavHandle_->gpuHandle = pDxSRVHeap_->GetGPUDescriptorHandel(uavHandle_->heapIndex);

	/// resource create
	_dxDevice->GetDevice()->CreateUnorderedAccessView(bufferResource_.Get(), nullptr, &desc, uavHandle_->cpuHandle);
}

template<typename T>
inline void StructuredBuffer<T>::CreateAppendBuffer(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {
	isAppendBuffer_ = true;

	structureSize_ = sizeof(T);
	bufferSize_ = _size;
	totalSize_ = structureSize_ * bufferSize_;

	bufferResource_.CreateUAVResource(_dxDevice, _dxCommand, totalSize_);

	D3D12_RESOURCE_DESC counterDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	counterResource_.CreateCommittedResource(_dxDevice, &heapProperties, D3D12_HEAP_FLAG_NONE, &counterDesc, D3D12_RESOURCE_STATE_COMMON, nullptr);
	counterResource_.CreateBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, _dxCommand);

	{	/// カウンタリセット用アップロードバッファを用意(0を1つ)
		D3D12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT));
		D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		counterResetResource_.CreateCommittedResource(
			_dxDevice,
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&uploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr
		);

		UINT* mapped = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		counterResetResource_.Get()->Map(0, &readRange, reinterpret_cast<void**>(&mapped));
		*mapped = 0;
		counterResetResource_.Get()->Unmap(0, nullptr);
	}

	{	/// カウンタ読み取り用リードバックバッファ(非同期読み込み用)
		D3D12_RESOURCE_DESC readbackDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT));
		D3D12_HEAP_PROPERTIES readbackHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
		readbackResource_.CreateCommittedResource(_dxDevice,
			&readbackHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&readbackDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr
		);
	}

	/// UAV作成（カウンタバッファをセット）
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = static_cast<UINT>(bufferSize_);
	desc.Buffer.StructureByteStride = static_cast<UINT>(structureSize_);
	desc.Buffer.CounterOffsetInBytes = 0;
	desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	pDxSRVHeap_ = _dxSRVHeap;
	appendHandle_ = std::make_optional<Handle>();
	appendHandle_->heapIndex = pDxSRVHeap_->AllocateBuffer();
	appendHandle_->cpuHandle = pDxSRVHeap_->GetCPUDescriptorHandel(appendHandle_->heapIndex);
	appendHandle_->gpuHandle = pDxSRVHeap_->GetGPUDescriptorHandel(appendHandle_->heapIndex);

	_dxDevice->GetDevice()->CreateUnorderedAccessView(bufferResource_.Get(), counterResource_.Get(), &desc, appendHandle_->cpuHandle);


	/// SRV作成
	srvHandle_ = std::make_optional<Handle>();
	srvHandle_->heapIndex = pDxSRVHeap_->AllocateBuffer();
	srvHandle_->cpuHandle = pDxSRVHeap_->GetCPUDescriptorHandel(srvHandle_->heapIndex);
	srvHandle_->gpuHandle = pDxSRVHeap_->GetGPUDescriptorHandel(srvHandle_->heapIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(bufferSize_);
	srvDesc.Buffer.StructureByteStride = static_cast<UINT>(structureSize_);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	_dxDevice->GetDevice()->CreateShaderResourceView(bufferResource_.Get(), &srvDesc, srvHandle_->cpuHandle);

}


template<typename T>
inline void StructuredBuffer<T>::CreateSRVAndUAV(uint32_t _size, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap) {
	// リソースのサイズを計算
	structureSize_ = sizeof(T);
	bufferSize_ = _size;
	totalSize_ = structureSize_ * bufferSize_;

	// リソースの作成
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(
		totalSize_,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS // UAVを許可
	);
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	bufferResource_.CreateCommittedResource(
		_dxDevice,
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON, // 初期状態
		nullptr
	);

	// SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(bufferSize_);
	srvDesc.Buffer.StructureByteStride = static_cast<UINT>(structureSize_);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	srvHandle_ = std::make_optional<Handle>();
	srvHandle_->heapIndex = _dxSRVHeap->AllocateBuffer();
	srvHandle_->cpuHandle = _dxSRVHeap->GetCPUDescriptorHandel(srvHandle_->heapIndex);
	srvHandle_->gpuHandle = _dxSRVHeap->GetGPUDescriptorHandel(srvHandle_->heapIndex);

	_dxDevice->GetDevice()->CreateShaderResourceView(bufferResource_.Get(), &srvDesc, srvHandle_->cpuHandle);

	// UAVの作成
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = static_cast<UINT>(bufferSize_);
	uavDesc.Buffer.StructureByteStride = static_cast<UINT>(structureSize_);
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	uavHandle_ = std::make_optional<Handle>();
	uavHandle_->heapIndex = _dxSRVHeap->AllocateBuffer();
	uavHandle_->cpuHandle = _dxSRVHeap->GetCPUDescriptorHandel(uavHandle_->heapIndex);
	uavHandle_->gpuHandle = _dxSRVHeap->GetGPUDescriptorHandel(uavHandle_->heapIndex);

	_dxDevice->GetDevice()->CreateUnorderedAccessView(bufferResource_.Get(), nullptr, &uavDesc, uavHandle_->cpuHandle);

	// マッピング
	//bufferResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	//mappedDataArray_ = { mappedData_, bufferSize_ };

	// デスクリプタヒープのポインタを保存
	pDxSRVHeap_ = _dxSRVHeap;
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

	readbackResource_.CreateBarrier(D3D12_RESOURCE_STATE_COPY_DEST, _dxCommand);
	counterResource_.CreateBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, _dxCommand);

	/// DefaultHeapのCounterリソースからReadbackHeapにコピー
	cmdList->CopyResource(readbackResource_.Get(), counterResource_.Get());

	/// コマンドリストを閉じて実行し、フェンス待ちなどが必要（ここでは省略）
	_dxCommand->CommandExecuteAndWait();
	_dxCommand->CommandReset();
	_dxCommand->WaitForGpuComplete();

	/// CPUでマップして読み取り
	uint32_t* mapped = nullptr;
	CD3DX12_RANGE readRange(0, sizeof(UINT));
	readbackResource_.Get()->Map(0, &readRange, reinterpret_cast<void**>(&mapped));
	uint32_t count = *mapped;
	readbackResource_.Get()->Unmap(0, nullptr);

	return count;
}

template<typename T>
inline void StructuredBuffer<T>::SRVBindForGraphicsCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const {
	_cmdList->SetGraphicsRootDescriptorTable(_rootParameterIndex, srvHandle_->gpuHandle);
}

template<typename T>
inline void StructuredBuffer<T>::SRVBindForComputeCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const {
	_cmdList->SetComputeRootDescriptorTable(_rootParameterIndex, srvHandle_->gpuHandle);
}

template<typename T>
inline void StructuredBuffer<T>::UAVBindForGraphicsCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const {
	_cmdList->SetGraphicsRootDescriptorTable(_rootParameterIndex, uavHandle_->gpuHandle);
}

template<typename T>
inline void StructuredBuffer<T>::UAVBindForComputeCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const {
	_cmdList->SetComputeRootDescriptorTable(_rootParameterIndex, uavHandle_->gpuHandle);
}

template<typename T>
inline void StructuredBuffer<T>::AppendBindForGraphicsCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const {
	_cmdList->SetGraphicsRootDescriptorTable(_rootParameterIndex, appendHandle_->gpuHandle);
}

template<typename T>
inline void StructuredBuffer<T>::AppendBindForComputeCommandList(ID3D12GraphicsCommandList* _cmdList, UINT _rootParameterIndex) const {
	_cmdList->SetComputeRootDescriptorTable(_rootParameterIndex, appendHandle_->gpuHandle);
}

template<typename T>
inline void StructuredBuffer<T>::SetMappedData(size_t _index, const T& _setValue) {
	Assert(_index < bufferSize_, "out of range");
	mappedDataArray_[_index] = _setValue;
}

template<typename T>
inline const T& StructuredBuffer<T>::GetMappedData(size_t _index) const {
	return mappedDataArray_[_index];
}

template<typename T>
inline const DxResource& StructuredBuffer<T>::GetResource() const {
	return bufferResource_;
}

template<typename T>
inline const DxResource& StructuredBuffer<T>::GetCounterResource() const {
	return counterResource_;
}

template<typename T>
inline const DxResource& StructuredBuffer<T>::GetCounterResetResource() const {
	return counterResetResource_;
}

template<typename T>
inline const DxResource& StructuredBuffer<T>::GetReadbackResource() const {
	return readbackResource_;
}



template<typename T>
inline DxResource& StructuredBuffer<T>::GetResource() {
	return bufferResource_;
}

template<typename T>
inline DxResource& StructuredBuffer<T>::GetCounterResource() {
	return counterResource_;
}

template<typename T>
inline DxResource& StructuredBuffer<T>::GetCounterResetResource() {
	return counterResetResource_;
}

template<typename T>
inline DxResource& StructuredBuffer<T>::GetReadbackResource() {
	return readbackResource_;
}
