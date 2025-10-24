#pragma once

/// std
#include <cstdint>

/// ///////////////////////////////////////////////////
/// RTV, DSV, CBV_SRV_UAV のHeapの上限値を定義
/// ///////////////////////////////////////////////////
namespace DescriptorHeapLimits {

	static const uint32_t RTV = 16;
	static const uint32_t DSV = 1;

	/// Textureのサイズは AssetCollection.h で定義しているのでそれ以上の数を確保する
	/// MAX_TEXTURE_COUNT + Buffer分
	static const uint32_t CBV_SRV_UAV = 512;

} /// namespace
