#pragma once

/// engine
#include "IDxDescriptorHeap.h"

/// /////////////////////////////////////////////////
/// RTVHeap
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class DxRTVHeap
 * @brief レンダーターゲットビュー（RTV）用D3D12ディスクリプタヒープの生成と管理を担当するクラス
 */
class DxRTVHeap final : public IDxDescriptorHeap {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ。上限数を指定します。
	 * @param _dxDevice デバイスオブジェクトへのポインタ
	 * @param _maxHeapSize 管理するRTVの上限数
	 */
	DxRTVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize);

	/**
	 * @brief デストラクタ
	 */
	~DxRTVHeap();

	/**
	 * @brief RTV用のディスクリプタヒープ（ID3D12DescriptorHeap）をデバイスを通じて新規に生成します。
	 */
	void Initialize() override;
};


} /// ONEngine
