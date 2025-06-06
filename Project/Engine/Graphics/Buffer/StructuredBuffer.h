#pragma once

/// directX12
#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <cstdint>
#include <span>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
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

	void BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList);

	void SetMappedData(size_t _index, const T& _setValue);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================
	
	uint32_t                    srvDescriptorIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

	DxResource                  bufferResource_;
	T*                          mappedData_;
	std::span<T>                mappedDataArray_;

	size_t                      structureSize_;
	size_t                      totalSize_;
	size_t                      bufferSize_;


	DxSRVHeap*                  pDxSRVHeap_;
};


/// ===================================================
/// public : method definition
/// ===================================================

template<typename T>
inline StructuredBuffer<T>::StructuredBuffer() {}

template<typename T>
inline StructuredBuffer<T>::~StructuredBuffer() {
	pDxSRVHeap_->Free(srvDescriptorIndex_);
}


template<typename T>
inline void StructuredBuffer<T>::Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {

	/// bufferのサイズを計算
	structureSize_  = sizeof(T);
	bufferSize_     = _size;
	totalSize_      = structureSize_ * bufferSize_;

	/// bufferの生成
	bufferResource_.CreateResource(_dxDevice, totalSize_);

	/// desc setting
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format                       = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping      = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension                = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement          = 0;
	desc.Buffer.Flags                 = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements           = static_cast<UINT>(bufferSize_);
	desc.Buffer.StructureByteStride   = static_cast<UINT>(structureSize_);

	/// cpu, gpu handle initialize
	pDxSRVHeap_         = _dxSRVHeap;
	srvDescriptorIndex_ = pDxSRVHeap_->AllocateBuffer();
	cpuHandle_          = pDxSRVHeap_->GetCPUDescriptorHandel(srvDescriptorIndex_);
	gpuHandle_          = pDxSRVHeap_->GetGPUDescriptorHandel(srvDescriptorIndex_);

	/// resource create
	_dxDevice->GetDevice()->CreateShaderResourceView(bufferResource_.Get(), &desc, cpuHandle_);

	/// mapping
	bufferResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	mappedDataArray_ = { mappedData_, bufferSize_ };
}


template<typename T>
inline void StructuredBuffer<T>::BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	_commandList->SetGraphicsRootDescriptorTable(_rootParameterIndex, gpuHandle_);
}

template<typename T>
inline void StructuredBuffer<T>::SetMappedData(size_t _index, const T& _setValue) {
	Assert(_index < bufferSize_, "out of range");
	mappedDataArray_[_index] = _setValue;
}
