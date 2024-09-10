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

private:

	std::unique_ptr<Sprite> start_;
	std::unique_ptr<Sprite> end_;

	Vec3 offset_ = { 640.0f, 360.0f, 0.0f };
	Vec3 distance_ = {};

	Vec2 spriteSize_;
	Vec4 spriteColor_;

	float animationTime_ = 0.0f;
	float animationSpeed_ = 1.0f;

};