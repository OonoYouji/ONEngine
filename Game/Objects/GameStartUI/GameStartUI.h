#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameStartUI final : public BaseGameObject {
public:
	GameStartUI() { CreateTag(this); }
	~GameStartUI() {}

	void Initialize() override;
	void Update() override;
	void FrontSpriteDraw() override;

	void Debug() override;

	void SettingSprites();

	void RecalculateArrowPosition();

	bool GetIsGameStart() const {
		return isGameStart_;
	}

private:

	std::unique_ptr<Sprite> start_;
	std::unique_ptr<Sprite> end_;
	std::unique_ptr<Sprite> arrows_[2];

	Vec3 offset_ = { 640.0f, 360.0f, 0.0f };
	Vec3 distance_ = {};

	Vec2 spriteSize_;
	Vec4 spriteColor_;

	float animationTime_ = 0.0f;
	float animationSpeed_ = 1.0f;

	float arrowOffset_ = 10.0f;
	Vec2 arrowSpriteSize_;

	/// ゲームを始めるか、終わるかのモード
	uint8_t currentSelectMode_ = 0;

	bool isGameStart_ = false;
	float startAnimationTime_ = 0.0f;

	/// 横から出てくる心臓と手
	class Heart* pHeart_ = nullptr;
	class Hand* pHand_ = nullptr;

};