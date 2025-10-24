#include "ByteAddressBuffer.h"

/// engine
#include "Engine/Core/Utility/Utility.h"

ByteAddressBuffer::ByteAddressBuffer() {}
ByteAddressBuffer::~ByteAddressBuffer() {
	/// Heapの解放
	if (pDxSRVHeap_) {
		pDxSRVHeap_->Free(srvDescriptorIndex_);
	}
}

void ByteAddressBuffer::Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap) {
	/// ----- Bufferを作成する ----- ///

	/// bufferのサイズを計算
	size_t bitSize = sizeof(uint32_t);
	bufferSize_ = _size;
	totalSize_ = bitSize * bufferSize_;

	/// bufferの生成
	bufferResource_.CreateResource(_dxDevice, totalSize_);

	/// desc setting
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
	desc.Buffer.NumElements = static_cast<UINT>(bufferSize_);
	desc.Buffer.StructureByteStride =0;

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

void ByteAddressBuffer::SetMappedData(size_t _index, uint32_t _value) {
	Assert(_index < bufferSize_, "out of range");
	mappedDataArray_[_index] = _value;
}

void ByteAddressBuffer::BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList) {
	_commandList->SetGraphicsRootDescriptorTable(_rootParameterIndex, gpuHandle_);
}
