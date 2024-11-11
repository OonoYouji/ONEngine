#pragma once

#include <memory>

#include "ComponentManager/Base/BaseComponent.h"
#include "ComponentManager/Transform/Transform.h"
#include "GraphicManager/Drawer/Sprite/Sprite.h"


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
	void Draw()   override;

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
	/// 表示する画像の左上座標をセットする
	/// </summary>
	/// <param name="position"></param>
	void SetUVPosition(const Vec2& position);

	/// <summary>
	/// 表示する画像の大きさをセットする
	/// pixel数で指定 256x256中の100x50など
	/// </summary>
	/// <param name="size"></param>
	void SetUVSize(const Vec2& size);

	/// <summary>
	/// 表示する画像の回転をセットする
	/// </summary>
	/// <param name="rotate"></param>
	void SetUVRotate(float rotate);

	/// <summary>
	/// テクスチャの色をセットする
	/// </summary>
	/// <param name="_color">: テクスチャの色 </param>
	void SetColor(const Vec4& _color);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	Transform*				pTransform_ = nullptr;
	std::unique_ptr<Sprite> sprite_		= nullptr;

};