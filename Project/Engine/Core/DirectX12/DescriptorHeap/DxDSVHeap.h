#pragma once

/// engine
#include "IDxDescriptorHeap.h"

/// /////////////////////////////////////////////////
/// DSVHeap
/// /////////////////////////////////////////////////
namespace ONEngine {

/**
 * @class DxDSVHeap
 * @brief デプスステンシルビュー（DSV）用D3D12ディスクリプタヒープの生成と管理を担当するクラス
 */
class DxDSVHeap final : public IDxDescriptorHeap {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief コンストラクタ。上限数を指定します。
	 * @param _dxDevice デバイスオブジェクトへのポインタ
	 * @param _maxHeapSize 管理するDSVの上限数
	 */
	DxDSVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize);

	/**
	 * @brief デストラクタ
	 */
	~DxDSVHeap();

	/**
	 * @brief DSV用のディスクリプタヒープ（ID3D12DescriptorHeap）をデバイスを通じて新規に生成します。
	 */
	void Initialize() override;
};


} /// ONEngine
