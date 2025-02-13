#include "DxDescriptorHeap.h"

/// lib
#include "Engine/Core/Utility/DebugTools/Assert.h"
#include "Engine/Core/Utility/DebugTools/Log.h"


namespace {
	/// ===================================================
	/// DescriptorHeapを作成する関数
	/// ===================================================
	ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* _device, D3D12_DESCRIPTOR_HEAP_TYPE _heapType, UINT _numDescriptors, bool shaderVisible) {
		ComPtr<ID3D12DescriptorHeap> heap;
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Type = _heapType;
		desc.NumDescriptors = _numDescriptors;
		desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		HRESULT result = _device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
		Assert(SUCCEEDED(result), "miss created descriptor heap");

		return heap;
	}
}



void IDxDescriptorHeap::Free(uint32_t _index) {
	auto itr = std::find(spaceIndex_.begin(), spaceIndex_.end(), _index);
	if(itr == spaceIndex_.end()) {
		spaceIndex_.push_back(_index);
	}
}

uint32_t IDxDescriptorHeap::Allocate() {
	/// 削除されたindexがあれば再利用する
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




template<>
void DxDescriptorHeap<DescriptorHeapType_CBV_SRV_UAV>::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxHeapSize_, true);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	useIndex_ = 0;

	Log("dx descriptor heap cbv,srv,uav create success!!");
}

template<>
void DxDescriptorHeap<DescriptorHeapType_RTV>::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxHeapSize_, false);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	useIndex_ = 0;

	Log("dx descriptor heap rtv create success!!");
}

template<>
void DxDescriptorHeap<DescriptorHeapType_DSV>::Initialize() {
	ID3D12Device* pDevice = pDxDevice_->GetDevice();

	descriptorHeap_ = CreateHeap(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxHeapSize_, false);
	descriptorSize_ = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	useIndex_ = 0;

	Log("dx descriptor heap dsv create success!!");
}
