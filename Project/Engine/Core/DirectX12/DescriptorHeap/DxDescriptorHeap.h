#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>
#include <deque>

/// engine
#include "../ComPtr/ComPtr.h"
#include "../Device/DxDevice.h"

/// ===================================================
/// DescriptorHeapの基底クラス
/// ===================================================
class IDxDescriptorHeap {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	IDxDescriptorHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize) : pDxDevice_(_dxDevice), kMaxHeapSize_(_maxHeapSize) {}
	virtual ~IDxDescriptorHeap() {}


	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;


	/// <summary>
	/// 特定のDescriptorを解放する
	/// </summary>
	/// <param name="_index"> : Allocate()でゲットしたindex </param>
	void Free(uint32_t _index);

	/// <summary>
	/// DescriptorHeapのindexをゲットする
	/// </summary>
	/// <returns> return : DescriptorHeapのindex </returns>
	uint32_t Allocate();

	/// <summary>
	/// コマンドリストにDescriptorHeapをセットする
	/// </summary>
	/// <param name="_commandList"></param>
	void BindToCommandList(ID3D12GraphicsCommandList* _commandList);


protected:

	/// ===================================================
	/// protected : objects
	/// ===================================================

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;


	const uint32_t               kMaxHeapSize_;             /// heapのmax
	uint32_t                     descriptorSize_;           /// heapの1つあたりsize

	uint32_t                     useIndex_;
	std::deque<uint32_t>         spaceIndex_;               /// 解放された後の空きindex

	/// other class pointer
	DxDevice*                    pDxDevice_ = nullptr;


public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// cpuHandleのゲッタ
	/// </summary>
	/// <param name="_index"> : </param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandel(uint32_t _index) const;

	/// <summary>
	/// gpuHandleのゲッタ
	/// </summary>
	/// <param name="_index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandel(uint32_t _index) const;

	/// <summary>
	/// DescriptorHeapのゲッター
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetHeap() const { return descriptorHeap_.Get(); }

	/// <summary>
	/// DescriptorHeapの最大数をゲットする
	/// </summary>
	/// <returns> return : ヒープのマックスのサイズを得る </returns>
	uint32_t GetMaxHeapSize() const { return kMaxHeapSize_; }

	/// <summary>
	/// DescriptorHeapの使用済みの
	/// </summary>
	/// <returns></returns>
	uint32_t GetUsedIndexCount() const { return useIndex_ - static_cast<uint32_t>(spaceIndex_.size()); }


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	IDxDescriptorHeap(const IDxDescriptorHeap&)            = delete;
	IDxDescriptorHeap(IDxDescriptorHeap&&)                 = delete;
	IDxDescriptorHeap& operator=(const IDxDescriptorHeap&) = delete;
	IDxDescriptorHeap& operator=(IDxDescriptorHeap&&)      = delete;
};


/// ===================================================
/// DesacriptorHeapの種類
/// ===================================================
enum DescriptorHeapType {
	DescriptorHeapType_RTV,                        /// render target view用
	DescriptorHeapType_CBV_SRV_UAV,                /// cbv, srv, uav用
	DescriptorHeapType_DSV,                        /// depth stencil view用
	DescriptorHeapType_COUNT                       /// 種類数
};


/// ===================================================
/// dx12のdescriptor heapを管理するクラス
/// ===================================================
template<DescriptorHeapType T>
class DxDescriptorHeap : public IDxDescriptorHeap {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DxDescriptorHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize) : IDxDescriptorHeap(_dxDevice, _maxHeapSize) {}
	~DxDescriptorHeap() {}

	void Initialize() override;

public:

	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// <summary>
	/// DescriptorHeapの最大数を取得する
	/// </summary>
	/// <returns> return : ヒープのマックスのサイズを得る </returns>
	uint32_t GetMaxHeapSize() const { return kMaxHeapSize_; }

	/// <summary>
	/// DescriptorHeapの使用済みの数を取得する
	/// </summary>
	/// <returns></returns>
	uint32_t GetUsedIndexCount() const { return useIndex_ - static_cast<uint32_t>(spaceIndex_.size()); }


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	DxDescriptorHeap(const DxDescriptorHeap&)            = delete;
	DxDescriptorHeap(DxDescriptorHeap&&)                 = delete;
	DxDescriptorHeap& operator=(const DxDescriptorHeap&) = delete;
	DxDescriptorHeap& operator=(DxDescriptorHeap&&)      = delete;
};


/// @brief DescriptorHeapの生成
/// @param _device device へのポインタ
/// @param _type heapの種類
/// @param _numDescriptors descriptorの個数 
/// @param _isShaderVisible shader visibleかどうか
/// @return 生成された DescriptorHeap
ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* _device, D3D12_DESCRIPTOR_HEAP_TYPE _type, uint32_t _numDescriptors, bool _isShaderVisible);
