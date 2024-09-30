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

	/// <summary>
	/// 表示する画像の左上座標を決定する
	/// </summary>
	/// <param name="position"></param>
	void SetUVPosition(const Vec2& position);

	/// <summary>
	/// 表示する画像の大きさをセットする
	/// pixel数で指定 256x256中の100x50など
	/// </summary>
	/// <param name="size"></param>
	void SetUVSize(const Vec2& size);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Transform*				pTransform_ = nullptr;
	std::unique_ptr<Sprite> sprite_		= nullptr;
	SpriteRenderType		type_		= SpriteRenderType::Front;

	Vec2 uvSize_     = {};
	Vec2 uvPosition_ = {};

};