#pragma once

/// directX12
#include <d3d12.h>
#include <wrl/client.h>

/// std
#include <cstdint>
#include <span>

/// engine
#include "Core/ONEngine.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxCommon.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDevice.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxDescriptorHeap.h"
#include "GraphicManager/GraphicsEngine/DirectX12/DxResourceCreator.h"


/// ===================================================
/// ストラクチャードバッファ用クラス
/// ===================================================
template <typename T>
class DxStructuredBuffer final {
public:
	
	/// ===================================================
	/// public : methods
	/// ===================================================
	
	DxStructuredBuffer();
	~DxStructuredBuffer();

	/// <summary>
	/// DxStructuredBufferを作成する
	/// </summary>
	/// <param name="_size"> : 個数 </param>
	void Create(uint32_t _size);

	/// <summary>
	/// コマンドリストにバインドする
	/// </summary>
	/// <param name="_rootParameterIndex"> : ルートパラメータのインデックス </param>
	/// <param name="_commandList">        : バインド先のコマンドリスト    </param>
	void BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList);

	/// <summary>
	/// 値をセットする
	/// </summary>
	/// <param name="_index">    : セットする配列インデックス </param>
	/// <param name="_setValue"> : セットする値             </param>
	void SetMappedData(size_t _index, const T& _setValue);

private:

	/// ===================================================
	/// private : objects
	/// ===================================================
	
	uint32_t                    srvDescriptorIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

	ComPtr<ID3D12Resource>      bufferResource_;
	T*                          mappedData_;
	std::span<T>                mappedDataArray_;

	size_t                      structureSize_;
	size_t                      totalSize_;
	size_t                      bufferSize_;

};


/// ===================================================
/// public : method definition
/// ===================================================

template<typename T>
inline DxStructuredBuffer<T>::DxStructuredBuffer() {}

template<typename T>
inline DxStructuredBuffer<T>::~DxStructuredBuffer() {
	auto srvHeap = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();
	srvHeap->Free(srvDescriptorIndex_);
}


template<typename T>
inline void DxStructuredBuffer<T>::Create(uint32_t _size) {

	/// bufferのサイズを計算
	structureSize_  = sizeof(T);
	bufferSize_     = _size;
	totalSize_      = structureSize_ * bufferSize_;

	/// bufferの生成
	bufferResource_ = ONE::DxResourceCreator::CreateResource(totalSize_);

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
	auto srvHeap        = ONEngine::GetDxCommon()->GetSRVDescriptorHeap();
	srvDescriptorIndex_ = srvHeap->Allocate();
	cpuHandle_          = srvHeap->GetCPUDescriptorHandel(srvDescriptorIndex_);
	gpuHandle_          = srvHeap->GetGPUDescriptorHandel(srvDescriptorIndex_);

	/// resource create
	ONE::DxDevice* dxDevice = ONEngine::GetDxCommon()->GetDxDevice();
	dxDevice->GetDevice()->CreateShaderResourceView(bufferResource_.Get(), &desc, cpuHandle_);

	/// mapping
	bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	mappedDataArray_ = { mappedData_, bufferSize_ };
}


template<typename T>
inline void DxStructuredBuffer<T>::BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	_commandList->SetGraphicsRootDescriptorTable(_rootParameterIndex, gpuHandle_);
}

template<typename T>
inline void DxStructuredBuffer<T>::SetMappedData(size_t _index, const T& _setValue) {
	Assert(_index < bufferSize_, "out of range");

	mappedDataArray_[_index] = _setValue;
}
