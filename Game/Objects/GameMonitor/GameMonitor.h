#pragma once

#include <GameObjectManager.h>

class GameMonitor final : public BaseGameObject {
public:

	GameMonitor() {
		CreateTag(this);
	}
	~GameMonitor() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:

	Model* tv_ = nullptr;
	Model* screen_ = nullptr;

	Material screenMaterial_;

};