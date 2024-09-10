#pragma once

#include <memory>

#include <GameObjectManager.h>
#include <AudioManager.h>

class GameOperationUI final : public BaseGameObject {
public:

	GameOperationUI() { CreateTag(this); }
	~GameOperationUI() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Model* paper_ = nullptr;
	Model* binder_ = nullptr;
	Material paperMaterial_;
};