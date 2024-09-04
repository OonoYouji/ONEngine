#pragma once

#include <GameObjectManager.h>

class Wire final : public BaseGameObject {
public:

	Wire() {
		CreateTag(this);
	}
	~Wire() {}

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:

	Model* model_ = nullptr;
};