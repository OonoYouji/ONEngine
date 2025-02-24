#pragma once

/// engine
#include "DxDescriptorHeap.h"

/// ===================================================
/// DSVHeap
/// ===================================================
class DxDSVHeap final : public IDxDescriptorHeap {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DxDSVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize) 
		: IDxDescriptorHeap(_dxDevice, _maxHeapSize) {}

	~DxDSVHeap() {}

	
	/// @brief 初期化
	void Initialize() override;


private:

	/// ===================================================
	/// private : objects
	/// ===================================================



};

