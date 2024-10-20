#pragma once

#include "GameObjectManager/GameObjectManager.h"

class RetryUI : public BaseGameObject {
public:

	RetryUI();
	~RetryUI();

	void Initialize() override;
	void Update()     override;

private:
	class SpriteRenderer* spriteRenderer_ = nullptr;
};
