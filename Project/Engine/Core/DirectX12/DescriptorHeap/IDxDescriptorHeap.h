#pragma once

/// directX
#include <d3d12.h>

/// std
#include <cstdint>
#include <deque>

/// engine
#include "../ComPtr/ComPtr.h"
#include "../Device/DxDevice.h"



/// /////////////////////////////////////////////////
/// DescriptorHeapの種類
/// /////////////////////////////////////////////////
enum DescriptorHeapType {
	DescriptorHeapType_RTV,                        /// render target view用
	DescriptorHeapType_CBV_SRV_UAV,                /// cbv, srv, uav用
	DescriptorHeapType_DSV,                        /// depth stencil view用
	DescriptorHeapType_COUNT                       /// 種類数
};



/// /////////////////////////////////////////////////
/// DescriptorHeapの基底クラス
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class IDxDescriptorHeap
 * @brief 各種D3D12ディスクリプタヒープ（RTV, DSV, CBV/SRV/UAV）の割当て・解放、およびリサイクルを管理する抽象基底クラス
 */
class IDxDescriptorHeap {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ。ヒープの上限サイズを設定します。
	 * @param _dxDevice デバイスポインタ
	 * @param _maxHeapSize このヒープで管理するディスクリプタの最大数
	 */
	IDxDescriptorHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize);

	/**
	 * @brief 仮想デストラクタ
	 */
	virtual ~IDxDescriptorHeap() = default;

	/**
	 * @brief 純粋仮想初期化関数。派生クラスで実際にヒープを生成します。
	 */
	virtual void Initialize() = 0;

	/**
	 * @brief 使用済みとなったディスクリプタインデックスを解放し、再利用キュー（空きリスト）に追加します。
	 * @param _index 解放するディスクリプタインデックス
	 */
	void Free(uint32_t _index);

	/**
	 * @brief 新しいディスクリプタ（ビュー）の割り当てを行い、そのインデックスを返します。
	 * @return 割り当てられたディスクリプタインデックス
	 */
	uint32_t Allocate();

	/**
	 * @brief シェーダ参照可能なヒープをグラフィックスコマンドリストに設定します。
	 * @param _commandList 設定先のコマンドリストポインタ
	 */
	void BindToCommandList(ID3D12GraphicsCommandList* _commandList);


protected:
	/// ===================================================
	/// protected : objects
	/// ===================================================

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	const uint32_t               kMaxHeapSize_;             ///< heapのmax
	uint32_t                     descriptorSize_;           ///< heapの1つあたりsize

	uint32_t                     useIndex_;
	std::deque<uint32_t>         spaceIndex_;               ///< 解放された後の空きindex

	DxDevice*                    pDxDevice_ = nullptr;


public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/**
	 * @brief 指定されたインデックスのCPUディスクリプタハンドルを取得します。
	 * @param _index ディスクリプタインデックス
	 * @return D3D12_CPU_DESCRIPTOR_HANDLE
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandel(uint32_t _index) const;

	/**
	 * @brief 指定されたインデックスのGPUディスクリプタハンドルを取得します。
	 * @param _index ディスクリプタインデックス
	 * @return D3D12_GPU_DESCRIPTOR_HANDLE
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandel(uint32_t _index) const;

	/**
	 * @brief 内部で管理している生ディスクリプタヒープを取得します。
	 * @return ID3D12DescriptorHeapポインタ
	 */
	ID3D12DescriptorHeap* GetHeap() const;

	/**
	 * @brief ディスクリプタヒープの最大容量を取得します。
	 * @return 最大容量
	 */
	uint32_t GetMaxHeapSize() const;

	/**
	 * @brief 現在アクティブに割り当てられているディスクリプタの総数を取得します。
	 * @return 使用中ディスクリプタ数
	 */
	uint32_t GetUsedIndexCount() const;


private:

	/// ===================================================
	/// private : copy delete
	/// ===================================================

	IDxDescriptorHeap(const IDxDescriptorHeap&)            = delete;
	IDxDescriptorHeap(IDxDescriptorHeap&&)                 = delete;
	IDxDescriptorHeap& operator=(const IDxDescriptorHeap&) = delete;
	IDxDescriptorHeap& operator=(IDxDescriptorHeap&&)      = delete;
};


/**
 * @brief 生のD3D12ディスクリプタヒープを生成するための汎用ヘルパー関数です。
 * @param _device 生成に使用するD3D12デバイス
 * @param _type ヒープの種類（D3D12_DESCRIPTOR_HEAP_TYPE）
 * @param _numDescriptors 確保するディスクリプタ数
 * @param _isShaderVisible シェーダから直接参照可能にするかどうか
 * @return 生成されたヒープオブジェクトのComPtr
 */
ComPtr<ID3D12DescriptorHeap> CreateHeap(ID3D12Device* _device, D3D12_DESCRIPTOR_HEAP_TYPE _type, uint32_t _numDescriptors, bool _isShaderVisible);

} /// ONEngine
