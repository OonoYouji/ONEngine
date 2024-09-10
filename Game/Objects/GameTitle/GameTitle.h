#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameTitle final : public BaseGameObject {
public:

	GameTitle() { CreateTag(this); }
	~GameTitle() {}

	void Initialize() override;
	void Update() override;
	void FrontSpriteDraw() override;

	void Debug() override;
private:
	std::unique_ptr<Sprite> title_;
	Vec3 titlePosition_;
	Vec2 titleSize_;
	Vec4 titleColor_;
};