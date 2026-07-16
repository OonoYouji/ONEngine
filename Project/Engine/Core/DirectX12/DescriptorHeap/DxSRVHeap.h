#pragma once

/// engine
#include "IDxDescriptorHeap.h"

/// /////////////////////////////////////////////////
/// DescriptorHeapの基底クラス
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class DxSRVHeap
 * @brief シェーダ参照可能なヒープ（CBV/SRV/UAV）の割当て・区分（Texture用、UAVテクスチャ用、Buffer用）を細分化して管理するクラス
 */
class DxSRVHeap final : public IDxDescriptorHeap {

	struct HeapData {
		uint32_t usedIndex;
		uint32_t startIndex;
		uint32_t heapSize;
		std::deque<uint32_t> spaceIndex;
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ。Buffer用とTexture用の合計割当て数を考慮して生成します。
	 * @param _dxDevice デバイスポインタ
	 * @param _bufferHeapSize バッファ用ディスクリプタ最大数
	 * @param _textureHeapSize テクスチャ用ディスクリプタ最大数
	 */
	DxSRVHeap(DxDevice* _dxDevice, uint32_t _bufferHeapSize, uint32_t _textureHeapSize);

	/**
	 * @brief デストラクタ
	 */
	~DxSRVHeap();

	/**
	 * @brief デバイスを通じて、シェーダ可視フラグを持った CBV/SRV/UAV 用ディスクリプタヒープを生成・初期化します。
	 */
	void Initialize() override;
	
	/**
	 * @brief 静的テクスチャSRV用の空きディスクリプタ領域を割り当て、その全体ヒープ内インデックスを返します。
	 * @return インデックス
	 */
	uint32_t AllocateTexture();

	/**
	 * @brief UAVテクスチャ（コンピュート書込み用など）用の空きディスクリプタ領域を割り当て、その全体ヒープ内インデックスを返します。
	 * @return インデックス
	 */
	uint32_t AllocateUAVTexture();

	/**
	 * @brief 定数バッファや構造化バッファなどのBuffer SRV/CBV用のディスクリプタ領域を割り当て、そのインデックスを返します。
	 * @return インデックス
	 */
	uint32_t AllocateBuffer();


	/**
	 * @brief テクスチャSRV領域の先頭GPUディスクリプタハンドルを取得します。
	 * @return D3D12_GPU_DESCRIPTOR_HANDLE
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVStartGPUHandle() const;

	/**
	 * @brief テクスチャSRV領域の先頭CPUディスクリプタハンドルを取得します。
	 * @return D3D12_CPU_DESCRIPTOR_HANDLE
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVStartCPUHandle() const;


	/**
	 * @brief UAVテクスチャ領域の先頭GPUディスクリプタハンドルを取得します。
	 * @return D3D12_GPU_DESCRIPTOR_HANDLE
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVStartGPUHandle() const;

	/**
	 * @brief UAVテクスチャ領域の先頭CPUディスクリプタハンドルを取得します。
	 * @return D3D12_CPU_DESCRIPTOR_HANDLE
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVStartCPUHandle() const;


	/**
	 * @brief バッファビュー（CBV/SRV/UAV）領域の先頭GPUディスクリプタハンドルを取得します。
	 * @return D3D12_GPU_DESCRIPTOR_HANDLE
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetBufferStartGPUHandle() const;

	/**
	 * @brief バッファビュー領域の先頭CPUディスクリプタハンドルを取得します。
	 * @return D3D12_CPU_DESCRIPTOR_HANDLE
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetBufferStartCPUHandle() const;

	/// 基底クラスのAllocateは使用禁止
	uint32_t Allocate() = delete;

private:
	/// ===================================================`
	/// private : objects
	/// ===================================================`

	HeapData srvTextureHeapData_; /// SRVのTexture用
	HeapData uavTextureHeapData_; /// UAVのTexture用
	HeapData bufferHeapData_;

};

} /// ONEngine
