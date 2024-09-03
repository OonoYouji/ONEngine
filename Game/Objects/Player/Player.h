#pragma once

#include <GameObjectManager.h>
#include <ModelManager.h>

class Player final : public BaseGameObject {
public:

	Player();
	~Player() {}

	void Initialize() override;
	void Update() override;

	void Draw() override;

private:
	Model* model_ = nullptr;
};