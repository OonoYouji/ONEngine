#pragma once

/// std
#include <cstdint>

enum PostEffectFlags_ {
	PostEffectFlags_None      = 0,      ///< なし
	PostEffectFlags_Lighting  = 1 << 0, ///< ライティング
	PostEffectFlags_Grayscale = 1 << 1, ///< グレースケール
	PostEffectFlags_Bloom     = 1 << 2, ///< ブルーム
	PostEffectFlags_Outline   = 1 << 3, ///< アウトライン
};


struct Material {
	uint32_t postEffectFlags; ///< マテリアルID
};