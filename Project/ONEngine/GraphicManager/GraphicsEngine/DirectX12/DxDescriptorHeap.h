#pragma once

/// directX12
#include <wrl/client.h>
#include <d3d12.h>

/// std
#include <cstdint>
#include <deque>

/// lib
#include "Debug/Assert.h"

/// using
using namespace Microsoft::WRL;


namespace ONE {

	/// 前方宣言
	class DxCommon;

	/// ===================================================
	/// Descriptorの種類
	/// ===================================================
	enum class HeapType {
		RTV,
		CBV_SRV_UAV,
		DSV
	};

	/// ===================================================
	/// Descriptorの管理をするtemplateクラス
	/// ===================================================
	template <HeapType type>
	class DxDescriptorHeap {
	public:
		/// ===================================================
		/// public : methods
		/// ===================================================

		DxDescriptorHeap(uint32_t _maxHeapSize) : kMaxHeapSize_(_maxHeapSize) {}
		~DxDescriptorHeap() {}

		/// <summary>
		/// 初期化関数
		/// </summary>
		/// <param name="_dxCommon"> : dxCommonへのポインター </param>
		void Initialize(DxCommon* _dxCommon);


		void Free(uint32_t _index) {
			spaceIndex_.push_back(_index);
		}


		uint32_t Allocate() {

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


		/// <summary>
		/// cpuHandleのゲッタ
		/// </summary>
		/// <param name="_index"> : </param>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandel(uint32_t _index) {
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (descriptorSize_ * _index);
			return cpuHandle;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandel(uint32_t _index) {
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
			gpuHandle.ptr += (descriptorSize_ * _index);
			return gpuHandle;
		}


		/// <summary>
		/// コマンドリストにDescriptorHeapをセットする
		/// </summary>
		/// <param name="_commandList"></param>
		void BindToCommandList(ID3D12GraphicsCommandList* _commandList) {
			ID3D12DescriptorHeap* heaps[] = { descriptorHeap_.Get() };
			_commandList->SetDescriptorHeaps(1, heaps);
		}


		/// <summary>
		/// DescriptorHeapのゲッター
		/// </summary>
		/// <returns></returns>
		ID3D12DescriptorHeap* GetHeap() const { return descriptorHeap_.Get(); }

	private:

		ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

		/// heapのmaxと1つあたりsize
		const uint32_t kMaxHeapSize_;
		uint32_t       descriptorSize_;

		uint32_t useIndex_;
		std::deque<uint32_t> spaceIndex_; /// 解放された後の空きindex

		/// other class pointer
		DxCommon* pDxCommon_ = nullptr;
	};



	/// ===================================================
	/// DescriptorHeapの生成
	/// ===================================================
	ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

}