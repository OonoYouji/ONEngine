#pragma once

/// std
#include <cstdint>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

/// @brief ポストエフェクトの適用
enum PostEffectFlags_ {
	PostEffectFlags_None      = 0,      ///< なし
	PostEffectFlags_Lighting  = 1 << 0, ///< ライティング
	PostEffectFlags_Grayscale = 1 << 1, ///< グレースケール
	//PostEffectFlags_Bloom     = 1 << 2, ///< ブルーム
	//PostEffectFlags_Outline   = 1 << 3, ///< アウトライン
};


struct Material {
	uint32_t postEffectFlags; ///< ポストエフェクトのフラグ
	Vector4  color; ///< 色
};