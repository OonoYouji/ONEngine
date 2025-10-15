#pragma once

/// std
#include <string>

/// external
#include <nlohmann/json.hpp>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Graphics/Buffer/Data/Material.h"


class GraphicsResourceCollection;
class SpriteRenderer;
namespace COMP_DEBUG {
	void SpriteDebug(SpriteRenderer* _sr, GraphicsResourceCollection* _resourceCollection);
}

/// json serialize
void to_json(nlohmann::json& _j, const SpriteRenderer& _sr);
void from_json(const nlohmann::json& _j, SpriteRenderer& _sr);

/// ===================================================
/// sprite描画クラス
/// ===================================================
class SpriteRenderer final : public IComponent {
	friend class SpriteUpdateSystem;

	friend void COMP_DEBUG::SpriteDebug(SpriteRenderer* _sr, GraphicsResourceCollection* _resourceCollection);
	friend void to_json(nlohmann::json& _j, const SpriteRenderer& _sr);
	friend void from_json(const nlohmann::json& _j, SpriteRenderer& _sr);
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

	Material material_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setter ----- ///
	void SetColor(const Vector4& _color);

	/// ----- getter ----- ///
	const Vector4& GetColor() const;

	const Material& GetMaterial() const;
	Material& GetMaterial();

};

