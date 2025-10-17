#pragma once

/// engine
#include "IDxDescriptorHeap.h"

/// /////////////////////////////////////////////////
/// DSVHeap
/// /////////////////////////////////////////////////
class DxDSVHeap final : public IDxDescriptorHeap {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DxDSVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize);
	~DxDSVHeap();

	/// @brief 初期化
	void Initialize() override;
};

