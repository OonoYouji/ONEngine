#pragma once

/// engine
#include "IDxDescriptorHeap.h"

/// /////////////////////////////////////////////////
/// DescriptorHeapの基底クラス
/// /////////////////////////////////////////////////
class DxSRVHeap final : public IDxDescriptorHeap {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	DxSRVHeap(DxDevice* _dxDevice, uint32_t _maxHeapSize, uint32_t _textureHeapSize);
	~DxSRVHeap();

	/// @brief 初期化
	void Initialize() override;
	
	/// @brief Texture用のDescriptorHeapのIndexを取得する
	/// @return DescriptorHeapのIndex
	uint32_t AllocateTexture();

	/// @brief Buffer用のDescriptorHeapのIndexを取得する
	/// @return DescriptorHeapのIndex
	uint32_t AllocateBuffer();

	/// @brief 基底クラスのAllocateは使用禁止
	uint32_t Allocate() = delete;

private:
	/// ===================================================`
	/// private : objects
	/// ===================================================`

	uint32_t             textureUseIndex_;   ///< Texture用の使用済みIndex
	uint32_t             bufferUseIndex_;	 ///< Buffer用の使用済みIndex

	uint32_t             textureHeapSize_;   ///< texture用の heap size
	uint32_t             bufferHeapSize_;    ///< buffer用の heap size

	std::deque<uint32_t> textureSpaceIndex_; ///< 再利用可能なDescriptorのIndex List
	std::deque<uint32_t> bufferSpaceIndex_;  ///< 再利用可能なDescriptorのIndex List

};
