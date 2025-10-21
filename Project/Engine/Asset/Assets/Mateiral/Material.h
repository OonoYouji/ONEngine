#pragma once

/// std
#include <optional>

/// engine
#include "../Asset.h"
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Utility/Utility.h"

/// ShowGuiMaterialように前方宣言
class Material;


/// @brief Material用のGUI表示
void ShowGuiMaterial(const std::string& _label, Material* _material);

/// @brief デフォルトのマテリアルを生成
Material GenerateMaterial();

/// @brief Materialファイルを生成
/// @param _filepath 生成先のファイルパス
/// @param _material 参照するマテリアル nullptrならデフォルト値で生成
void GenerateMaterialFile(const std::string& _filepath, Material* _material);



/// ////////////////////////////////////////////////////////
/// マテリアル
/// ////////////////////////////////////////////////////////
class Material final : public IAsset {
	/// friend functions
	friend void ShowGuiMaterial(const std::string& _label, Material* _material);

public:
	/// ==================================================
	/// public : methods
	/// ==================================================

	Material();
	~Material();

	

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
};

