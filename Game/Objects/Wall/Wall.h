#pragma once

#include <GameObjectManager.h>


class Wall final : public BaseGameObject {
public:

	Wall() { CreateTag(this); }
	~Wall() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Model* model_ = nullptr;
};