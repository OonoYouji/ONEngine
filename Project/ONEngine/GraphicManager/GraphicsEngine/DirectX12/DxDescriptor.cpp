#include "DxDescriptor.h"

#include <assert.h>

#include "DxCommon.h"
#include "DxCommand.h"
#include "DxDoubleBuffer.h"


ONE::DxDescriptor::DxDescriptor() {}
ONE::DxDescriptor::~DxDescriptor() {
	rtvHeap_.Reset();
}



/// ===================================================
/// 初期化
/// ===================================================
void ONE::DxDescriptor::Initialize(ID3D12Device* device) {

	rtvHeap_ = CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 32, false);
	rtvHeapSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	srvHeap_ = CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	srvHeapSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	dsvHeap_ = CreateHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	dsvHeapSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dsvStartCpuHandle_ = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

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
/// SRVからCPUHandleを得る
/// ===================================================
D3D12_CPU_DESCRIPTOR_HANDLE ONE::DxDescriptor::GetSrvCpuHandle() {
	return GetCpuHandle(srvHeap_.Get(), srvHeapSize_, srvUsedCount_);
}


/// ===================================================
/// SRVからGPUHandleを得る
/// ===================================================
D3D12_GPU_DESCRIPTOR_HANDLE ONE::DxDescriptor::GetSrvGpuHandle() {
	return GetGpuHandle(srvHeap_.Get(), srvHeapSize_, srvUsedCount_);
}


/// ===================================================
/// SRVの使用カウントを増やす
/// ===================================================
void ONE::DxDescriptor::AddSrvUsedCount() {
	++srvUsedCount_;
}


/// ===================================================
/// DSVの先頭アドレス
/// ===================================================
D3D12_CPU_DESCRIPTOR_HANDLE ONE::DxDescriptor::GetDsvCpuHandle() {
	return dsvHeap_->GetCPUDescriptorHandleForHeapStart();;
}


/// ===================================================
/// SRVHeapをCommandListにセットする
/// ===================================================
void ONE::DxDescriptor::SetSRVHeap(ID3D12GraphicsCommandList* commandList) {
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
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
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (heapSize * index);
	return cpuHandle;
}


/// ===================================================
/// DescriptorHeapからGpuHandleを得る
/// ===================================================
D3D12_GPU_DESCRIPTOR_HANDLE ONE::DxDescriptor::GetGpuHandle(ID3D12DescriptorHeap* heap, uint32_t heapSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (heapSize * index);
	return gpuHandle;
}

