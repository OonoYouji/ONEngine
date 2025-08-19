#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Buffer/Data/Material.h"

/// ===================================================
/// sprite描画クラス
/// ===================================================
class SpriteRenderer final : public IComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderer();
	~SpriteRenderer();


private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Material material_; ///< マテリアルデータ
	std::string texturePath_; ///< テクスチャパス

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setter ----- ///
	void SetTexturePath(const std::string& _path);
	void SetColor(const Vector4& _color);

	/// ----- getter ----- ///
	const std::string& GetTexturePath() const;
	const Vector4& GetColor() const;

	const Material& GetMaterial() const;
	Material& GetMaterial(); 

};


class GraphicsResourceCollection;
namespace COMP_DEBUG {
	void SpriteDebug(SpriteRenderer* _sr, GraphicsResourceCollection* _resourceCollection);
	void SpriteTextureDebug(SpriteRenderer* _sr, std::string& _texturePath);
}

/// json serialize
void to_json(nlohmann::json& _j, const SpriteRenderer& _sr);
void from_json(const nlohmann::json& _j, SpriteRenderer& _sr);