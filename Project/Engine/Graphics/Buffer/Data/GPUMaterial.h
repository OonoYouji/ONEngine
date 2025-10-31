#pragma once

/// std
#include <cstdint>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Asset/Guid/Guid.h"
#include "UVTransform.h"

/// /////////////////////////////////////////////////
/// @brief ポストエフェクトの適用
/// /////////////////////////////////////////////////
enum PostEffectFlags_ {
	PostEffectFlags_None = 0,      ///< なし
	PostEffectFlags_Lighting = 1 << 0, ///< ライティング
	PostEffectFlags_Grayscale = 1 << 1, ///< グレースケール
	PostEffectFlags_EnvironmentReflection = 1 << 2, ///< 天球に合わせて環境反射を行う
};

/// /////////////////////////////////////////////////
/// @brief GPUで利用するマテリアルデータ構造体
/// /////////////////////////////////////////////////
struct GPUMaterial {
	UVTransform uvTransform;     /// UV変形
	Vector4     baseColor;       /// 色
	uint32_t    postEffectFlags; /// ポストエフェクトのフラグ
	int32_t     entityId;        /// エンティティID
	int32_t     baseTextureId;   /// ベーステクスチャID
	int32_t     normalTextureId; /// 法線テクスチャID
};


/// material
void to_json(nlohmann::json& _j, const GPUMaterial& _material);
void from_json(const nlohmann::json& _j, GPUMaterial& _material);
