#pragma once

/// std
#include <cstdint>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"

/// @brief ポストエフェクトの適用
enum PostEffectFlags_ {
	PostEffectFlags_None                  = 0,      ///< なし
	PostEffectFlags_Lighting              = 1 << 0, ///< ライティング
	PostEffectFlags_Grayscale             = 1 << 1, ///< グレースケール
	PostEffectFlags_EnvironmentReflection = 1 << 2, ///< 天球に合わせて環境反射を行う
};

struct UVTransform {
	Vector2 position; /// オフセット
	Vector2 scale = Vector2::kOne; /// スケール
	float   rotate;   /// 回転
	float   pad1[3];
};

struct Material {
	UVTransform uvTransform;     /// UV変形
	Vector4     baseColor;       /// 色
	uint32_t    postEffectFlags; /// ポストエフェクトのフラグ
	int32_t     entityId;        /// エンティティID
	int32_t     baseTextureId;   /// ベーステクスチャID
	int32_t     normalTextureId; /// 法線テクスチャID
};


/// uv transform
void to_json(nlohmann::json& _j, const UVTransform& _uvTransform);
void from_json(const nlohmann::json& _j, UVTransform& _uvTransform);

/// material
void to_json(nlohmann::json& _j, const Material& _material);
void from_json(const nlohmann::json& _j, Material& _material);