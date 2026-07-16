#include "IDxDescriptorHeap.h"

using namespace ONEngine;

/// lib
#include "Engine/Core/Utility/Tools/Assert.h"
#include "Engine/Core/Utility/Tools/Log.h"


/**
 * @brief 生のD3D12ディスクリプタヒープを生成するための汎用ヘルパー関数です。
 */
ComPtr<ID3D12DescriptorHeap> ONEngine::CreateHeap(ID3D12Device* _device, D3D12_DESCRIPTOR_HEAP_TYPE _heapType, uint32_t _numDescriptors, bool _isShaderVisible) {
	ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = _heapType;
	desc.NumDescriptors = _numDescriptors;
	desc.Flags = _isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = _device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
	Assert(SUCCEEDED(result), "miss created descriptor heap");

	return heap;
}



IDxDescriptorHeap::IDxDescriptorHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize)
	: pDxDevice_(_dxDevice), kMaxHeapSize_(_maxHeapSize) {}


/**
 * @brief 使用済みとなったディスクリプタインデックスを解放し、再利用キュー（空きリスト）に追加します。
 */
void IDxDescriptorHeap::Free(uint32_t _index) {
	/// ----- すでに解放されているIndexでなければ解放 ----- ///
	auto itr = std::find(spaceIndex_.begin(), spaceIndex_.end(), _index);
	if (itr == spaceIndex_.end()) {
		spaceIndex_.push_back(_index);
	}
}

/**
 * @brief 新しいディスクリプタ（ビュー）の割り当てを行い、そのインデックスを返します。
 */
uint32_t IDxDescriptorHeap::Allocate() {
	/// ----- 空きIndexがあればそれを返す ----- ///

	/// 削除された index があれば再利用する
	if (!spaceIndex_.empty()) {
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

/**
 * @brief シェーダ参照可能なヒープをグラフィックスコマンドリストに設定します。
 */
void IDxDescriptorHeap::BindToCommandList(ID3D12GraphicsCommandList* _commandList) {
	ID3D12DescriptorHeap* heaps[] = { descriptorHeap_.Get() };
	const UINT numHeaps = 1;
	_commandList->SetDescriptorHeaps(numHeaps, heaps);
}

/**
 * @brief 指定されたインデックスのCPUディスクリプタハンドルを取得します。
 */
D3D12_CPU_DESCRIPTOR_HANDLE IDxDescriptorHeap::GetCPUDescriptorHandel(uint32_t _index) const {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	cpuHandle.ptr += (descriptorSize_ * _index);
	return cpuHandle;
}

/**
 * @brief 指定されたインデックスのGPUディスクリプタハンドルを取得します。
 */
D3D12_GPU_DESCRIPTOR_HANDLE IDxDescriptorHeap::GetGPUDescriptorHandel(uint32_t _index) const {
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	gpuHandle.ptr += (descriptorSize_ * _index);
	return gpuHandle;
}

/**
 * @brief 内部で管理している生ディスクリプタヒープを取得します。
 */
ID3D12DescriptorHeap* IDxDescriptorHeap::GetHeap() const {
	return descriptorHeap_.Get();
}

/**
 * @brief ディスクリプタヒープの最大容量を取得します。
 */
uint32_t IDxDescriptorHeap::GetMaxHeapSize() const {
	return kMaxHeapSize_;
}

/**
 * @brief 現在アクティブに割り当てられているディスクリプタの総数を取得します。
 */
uint32_t IDxDescriptorHeap::GetUsedIndexCount() const {
	return useIndex_ - static_cast<uint32_t>(spaceIndex_.size());
}



