#pragma once

/// std
#include <cstdint>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

/// @brief ポストエフェクトの適用
enum PostEffectFlags_ {
	PostEffectFlags_None                  = 0,      ///< なし
	PostEffectFlags_Lighting              = 1 << 0, ///< ライティング
	PostEffectFlags_Grayscale             = 1 << 1, ///< グレースケール
	PostEffectFlags_EnvironmentReflection = 1 << 2, ///< 天球に合わせて環境反射を行う
};


struct Material {
	Vector4  color; ///< 色
	uint32_t postEffectFlags; ///< ポストエフェクトのフラグ
	int32_t  entityId;
};