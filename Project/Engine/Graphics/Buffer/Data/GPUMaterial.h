#pragma once

/// std
#include <cstdint>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "Engine/Core/Utility/Math/Vector4.h"
#include "Engine/Asset/Guid/Guid.h"

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
/// @brief UV変形用データ構造体
/// /////////////////////////////////////////////////
struct UVTransform {
	Vector2 position; /// オフセット
	Vector2 scale = Vector2::kOne; /// スケール
	float   rotate;   /// 回転
	float   pad1[3];
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


/// uv transform
void to_json(nlohmann::json& _j, const UVTransform& _uvTransform);
void from_json(const nlohmann::json& _j, UVTransform& _uvTransform);

/// material
void to_json(nlohmann::json& _j, const GPUMaterial& _material);
void from_json(const nlohmann::json& _j, GPUMaterial& _material);


/// Material Json変換
class CPUMaterial;
void to_json(nlohmann::json& _j, const CPUMaterial& _material);
void from_json(const nlohmann::json& _j, CPUMaterial& _material);


/// /////////////////////////////////////////////////
/// @brief CPU側のマテリアルクラス
/// /////////////////////////////////////////////////
class CPUMaterial {
	friend void to_json(nlohmann::json& _j, const CPUMaterial& _material);
	friend void from_json(const nlohmann::json& _j, CPUMaterial& _material);
public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	CPUMaterial();
	~CPUMaterial();

	/// @brief ThisからGPU用のマテリアルデータを生成する
	GPUMaterial ToGPUMaterial();

	/// @brief 所有者エンティティを設定します。
	/// @param _pEntity 所有者として設定する GameEntity オブジェクトへのポインタ。
	void SetOwnerEntity(class GameEntity* _pEntity);


	/// ==================================================
	/// public : objects
	/// ==================================================

	/// ----- material data ----- ///

	UVTransform              uvTransform;
	Vector4                  baseColor;
	uint32_t 			     postEffectFlags;
	std::pair<Guid, int32_t> baseTextureIdPair;
	std::pair<Guid, int32_t> normalTextureIdPair;


private:
	/// ==================================================
	/// private : objects
	/// ==================================================


	/// ----- other class ----- ///

	class GameEntity* pOwnerEntity_ = nullptr;
};


