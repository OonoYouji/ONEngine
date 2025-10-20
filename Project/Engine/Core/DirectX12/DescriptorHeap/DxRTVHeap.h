#pragma once

/// engine
#include "IDxDescriptorHeap.h"

/// /////////////////////////////////////////////////
/// RTVHeap
/// /////////////////////////////////////////////////
class DxRTVHeap final : public IDxDescriptorHeap {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DxRTVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize);
	~DxRTVHeap();

	/// @brief 初期化
	void Initialize() override;
};

