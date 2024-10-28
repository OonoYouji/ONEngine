#include "DescriptorHeap.h"

#include "../DxCommon.h"

#include "Debug/Assert.h"


using namespace ONE;

/// ===================================================
/// CBV_SRV_UAV Descriptor
/// ===================================================

void DescriptorHeap<HeapType::CBV_SRV_UAV>::Initialize(DxCommon* _dxCommon) {
	pDxCommon_ = _dxCommon;
	Assert(pDxCommon_, "dxCommon == nullptr !!!!");

	ID3D12Device* pDevice = pDxCommon_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxHeapSize_, true);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

template<HeapType type>
uint32_t ONE::DescriptorHeap<type>::Allocate() {
	int result = useIndex_;
	useIndex_++;
	return result;
}






/// ===================================================
/// DescriptorHeapを作成する関数
/// ===================================================
ComPtr<ID3D12DescriptorHeap> ONE::CreateHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = heapType;
	desc.NumDescriptors = numDescriptors;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
	Assert(SUCCEEDED(result), "miss created descriptor heap");

	return heap;
}
