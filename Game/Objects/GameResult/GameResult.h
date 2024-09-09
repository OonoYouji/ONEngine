#pragma once

#include <GameObjectManager.h>

class GameResult final : public BaseGameObject {
public:

	GameResult() { CreateTag(this); }
	~GameResult() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Model* paper_ = nullptr;
	Model* binder_ = nullptr;
};