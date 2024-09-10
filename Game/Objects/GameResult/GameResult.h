#pragma once

#include <GameObjectManager.h>
#include <Sprite.h>

class GameResult final : public BaseGameObject {
public:

	GameResult() { CreateTag(this); }
	~GameResult() {}

	void Initialize() override;
	void Update() override;

	void BackSpriteDraw() override;

private:
	std::unique_ptr<Sprite> sprite_;
};