#pragma once

/// std
#include <vector>
#include <span>

/// engine
#include "Engine/Core/DirectX12/ComPtr/ComPtr.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"
#include "Engine/Core/DirectX12/Device/DxDevice.h"
#include "Engine/Core/DirectX12/DescriptorHeap/DxSRVHeap.h"

/// //////////////////////////////////////////////////////
/// ByteAddressBuffer
/// //////////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class ByteAddressBuffer
 * @brief バイト単位でアドレス指定可能な汎用D3D12バッファ（ByteAddressBuffer）およびCPU側マッピング領域を管理するクラス
 */
class ByteAddressBuffer final {
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	/**
	 * @brief コンストラクタ
	 */
	ByteAddressBuffer();

	/**
	 * @brief デストラクタ。確保したSRV記述子ヒープ領域の解放を行います。
	 */
	~ByteAddressBuffer();

	/**
	 * @brief バイトアドレスバッファリソースの生成、SRVのヒープ登録、およびCPUへのマッピングを行います。
	 * @param _size 要素数（アライメントに合うサイズに内部切り上げされます）
	 * @param _dxDevice デバイスポインタ
	 * @param _dxSRVHeap SRV用のディスクリプタヒープポインタ
	 */
	void Create(uint32_t _size, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap);


	/**
	 * @brief マップされたデータ領域の指定インデックス位置に32ビット符号なし整数値を設定します。
	 * @param _index 32ビット要素のインデックス
	 * @param _value 書き込む値
	 */
	void SetMappedData(size_t _index, uint32_t _value);

	/**
	 * @brief グラフィックスコマンドリストにこのバッファのSRVビューをバインドします。
	 * @param _rootParameterIndex ルートパラメータのインデックス
	 * @param _commandList 対象のコマンドリストポインタ
	 */
	void BindToCommandList(UINT _rootParameterIndex, ID3D12GraphicsCommandList* _commandList);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	uint32_t                    srvDescriptorIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

	DxResource                  bufferResource_;
	uint32_t*                   mappedData_;
	std::span<uint32_t>         mappedDataArray_;

	size_t                      totalSize_;
	size_t                      bufferSize_;

	DxSRVHeap* pDxSRVHeap_;


};

} /// ONEngine
