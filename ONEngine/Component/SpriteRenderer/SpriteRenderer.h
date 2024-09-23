#pragma once

#include <memory>

#include <Component/Base/BaseComponent.h>
#include <Component/Transform/Transform.h>
#include <Sprite.h>


/// ===================================================
/// enum class : render type
/// ===================================================
enum class SpriteRenderType {
	Front, Back
};

/// ===================================================
/// スプライトの描画をするクラス
/// ===================================================
class SpriteRenderer final : public BaseComponent {
public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	SpriteRenderer() {}
	~SpriteRenderer() {}

	void Initialize() override;

	void Update() override;

	void BackSpriteDraw() override;
	void FrontSpriteDraw() override;

	void Debug() override;


	/// <summary>
	/// テクスチャのセット
	/// </summary>
	/// <param name="filePath"></param>
	void SetTexture(const std::string& filePath);

	/// <summary>
	/// アンカーポイントのセット
	/// </summary>
	/// <param name="anchor"></param>
	void SetAnchor(const Vec2& anchor);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Transform*				pTransform_ = nullptr;
	std::unique_ptr<Sprite> sprite_		= nullptr;
	SpriteRenderType		type_		= SpriteRenderType::Front;

};