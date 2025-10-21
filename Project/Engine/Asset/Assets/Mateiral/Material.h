#pragma once

/// std
#include <optional>

/// engine
#include "Engine/Asset/Guid/Guid.h"
#include "Engine/Core/Utility/Utility.h"

/// ShowGuiMaterialように前方宣言
class Material;


/// @brief Material用のGUI表示
void ShowGuiMaterial(const std::string& _label, Material* _material);


/// ////////////////////////////////////////////////////////
/// マテリアル
/// ////////////////////////////////////////////////////////
class Material final {
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

	Guid guid_; /// 自身のGuid

	Vector4             baseColor_;
	uint32_t            postEffectFlags_;
	std::optional<Guid> baseTextureGuid_;   /// ベーステクスチャのGUID
	std::optional<Guid> normalTextureGuid_; /// 法線テクスチャのGUID

};

