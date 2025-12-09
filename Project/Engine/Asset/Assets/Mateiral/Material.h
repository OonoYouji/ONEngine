#pragma once

/// std
#include <optional>

/// engine
#include "../Asset.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Utility/Utility.h"
#include "Engine/Graphics/Buffer/Data/UVTransform.h"

/// ShowGuiMaterialように前方宣言
class Material;


/// @brief デフォルトのマテリアルを生成
Material GenerateMaterial();

/// @brief Materialファイルを生成
/// @param _filepath 生成先のファイルパス
/// @param _material 参照するマテリアル nullptrならデフォルト値で生成
void GenerateMaterialFile(const std::string& _filepath, Material* _material);


/// Json変換
void from_json(const nlohmann::json& _j, Material& _material);
void to_json(nlohmann::json& _j, const Material& _material);


/// ////////////////////////////////////////////////////////
/// マテリアル
/// ////////////////////////////////////////////////////////
class Material final : public IAsset {
	/// friend functions
	friend void from_json(const nlohmann::json& _j, Material& _material);
	friend void to_json(nlohmann::json& _j, const Material& _material);

public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	Material();
	~Material();

	
	/// @brief BaseTextureのGuidを持っているかチェック
	/// @return true: 持っている, false: 持っていない
	bool HasBaseTexture() const;

	/// @brief BaseTextureのGuidを取得
	/// @return BaseTextureのGuid
	const Guid& GetBaseTextureGuid() const;

	/// @brief base textureのGuidを設定
	/// @param _guid base textureのGuid
	void SetBaseTextureGuid(const Guid& _guid);


	/// @brief 法線テクスチャのGuidを持っているかチェック
	/// @return true: 持っている, false: 持っていない
	bool HasNormalTexture() const;

	/// @brief 法線テクスチャのGuidを取得
	/// @return 法線テクスチャのGuid
	const Guid& GetNormalTextureGuid() const;

	/// @brief normal textureのGuidを設定
	/// @param _guid normal textureのGuid
	void SetNormalTextureGuid(const Guid& _guid);

private:
	/// ==================================================
	/// private : objects
	/// ==================================================

	std::optional<Guid> baseTextureGuid_;   /// ベーステクスチャのGUID
	std::optional<Guid> normalTextureGuid_; /// 法線テクスチャのGUID


public:
	/// ==================================================
	/// public : objects
	/// ==================================================

	Vector4             baseColor;
	uint32_t            postEffectFlags;
	UVTransform         uvTransform;
};

