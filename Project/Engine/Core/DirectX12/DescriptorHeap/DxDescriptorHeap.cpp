#include "DxDescriptorHeap.h"

/// lib
#include "Engine/Core/Utility/DebugTools/Assert.h"
#include "Engine/Core/Utility/DebugTools/Log.h"


ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* _device, D3D12_DESCRIPTOR_HEAP_TYPE _heapType, uint32_t _numDescriptors, bool _isShaderVisible) {
	ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = _heapType;
	desc.NumDescriptors = _numDescriptors;
	desc.Flags = _isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = _device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
	Assert(SUCCEEDED(result), "miss created descriptor heap");

	return heap;
}



void IDxDescriptorHeap::Free(uint32_t _index) {
	auto itr = std::find(spaceIndex_.begin(), spaceIndex_.end(), _index);
	if(itr == spaceIndex_.end()) {
		spaceIndex_.push_back(_index);
	}
}

uint32_t IDxDescriptorHeap::Allocate() {
	/// 削除された index があれば再利用する
	if(!spaceIndex_.empty()) {
		uint32_t index = spaceIndex_.front();
		spaceIndex_.pop_front();
		return index;
	}

	/// 上限を超えていないかチェック
	Assert(useIndex_ < kMaxHeapSize_, "useIndex >= kMaxHeapSize_;  over!!!");
	uint32_t result = useIndex_;
	useIndex_++;
	return result;
}

void IDxDescriptorHeap::BindToCommandList(ID3D12GraphicsCommandList* _commandList) {
	ID3D12DescriptorHeap* heaps[] = { descriptorHeap_.Get() };
	_commandList->SetDescriptorHeaps(1, heaps);
}

D3D12_CPU_DESCRIPTOR_HANDLE IDxDescriptorHeap::GetCPUDescriptorHandel(uint32_t _index) const {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (descriptorSize_ * _index);
	return cpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE IDxDescriptorHeap::GetGPUDescriptorHandel(uint32_t _index) const {
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (descriptorSize_ * _index);
	return gpuHandle;
}


