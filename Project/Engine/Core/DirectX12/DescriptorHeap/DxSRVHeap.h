#pragma once

/// engine
#include "DxDescriptorHeap.h"

/// ===================================================
/// DescriptorHeapの基底クラス
/// ===================================================
class DxSRVHeap final : public IDxDescriptorHeap {
public:

	/// ===================================================
	/// public : methods
	/// ===================================================

	DxSRVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize, uint32_t _textureHeapSize) 
		: IDxDescriptorHeap(_dxDevice, _maxHeapSize), textureHeapSize_(_textureHeapSize) {}
	~DxSRVHeap() {}

	void Initialize() override;
	
	uint32_t AllocateTexture();
	uint32_t AllocateBuffer();

	uint32_t Allocate() = delete;
private:

	/// ===================================================`
	/// private : objects
	/// ===================================================`

	uint32_t             textureUseIndex_;   ///< 
	uint32_t             bufferUseIndex_;	 ///< 

	uint32_t             textureHeapSize_;   ///< texture用の heap size
	uint32_t             bufferHeapSize_;    ///< buffer用の heap size

	std::deque<uint32_t> textureSpaceIndex_; ///< 再利用可能なDescriptorのIndex List
	std::deque<uint32_t> bufferSpaceIndex_;  ///< 再利用可能なDescriptorのIndex List

};
