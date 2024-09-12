#pragma once

#include <GameObjectManager.h>

class Tile final : public BaseGameObject {
public:

	Tile() { CreateTag(this); }
	~Tile() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Model* model_ = nullptr;
};