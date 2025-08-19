#pragma once

/// std
#include <string>

/// engine
#include "../../Interface/IComponent.h"
#include "Engine/Core/Utility/Math/Vector4.h"

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

	Vector4 color_;
	std::string texturePath_;


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

};


namespace COMP_DEBUG {

	void SpriteDebug(SpriteRenderer* _sr);

	void SpriteTextureDebug(SpriteRenderer* _sr, std::string& _texturePath);

}