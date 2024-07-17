#include <DxDescriptor.h>

#include <assert.h>

#include <DxDoubleBuffer.h>


ONE::DxDescriptor::DxDescriptor() {}
ONE::DxDescriptor::~DxDescriptor() {
	rtvHeap_.Reset();
}



/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxDescriptor::Initialize(ID3D12Device* device) {

	rtvHeap_ = CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	rtvHeapSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


}


/// ===================================================
/// RTVからCPUHandleを得る
/// ===================================================
D3D12_CPU_DESCRIPTOR_HANDLE ONE::DxDescriptor::GetRtvCpuHandle() {
	return GetCpuHandle(rtvHeap_.Get(), rtvHeapSize_, rtvUsedCount_);
}


/// ===================================================
/// RTVの使用カウントを増やす
/// ===================================================
void ONE::DxDescriptor::AddRtvUsedCount() {
	++rtvUsedCount_;
}


/// ===================================================
/// DescriptorHeapの生成
/// ===================================================
ComPtr<ID3D12DescriptorHeap> ONE::DxDescriptor::CreateHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = heapType;
	desc.NumDescriptors = numDescriptors;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
	assert(SUCCEEDED(result));

	return heap;
}


/// ===================================================
/// DescriptorHeapからCpuHandleを得る
/// ===================================================
D3D12_CPU_DESCRIPTOR_HANDLE ONE::DxDescriptor::GetCpuHandle(ID3D12DescriptorHeap* heap, uint32_t heapSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = heap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (heapSize * index);
	return handleCPU;
}
