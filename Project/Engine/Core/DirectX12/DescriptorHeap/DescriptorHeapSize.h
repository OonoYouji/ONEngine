#pragma once

/// std
#include <cstdint>

/// ///////////////////////////////////////////////////
/// RTV, DSV, CBV_SRV_UAV のHeapの上限値を定義
/// ///////////////////////////////////////////////////
namespace DescriptorHeapLimits {

	static const uint32_t RTV = 64;
	static const uint32_t DSV = 8;

	/// Textureのサイズは AssetCollection.h で定義しているのでそれ以上の数を確保する
	/// MAX_TEXTURE_COUNT + Buffer分
	static const uint32_t CBV_SRV_UAV = 2048;

} /// namespace
