#pragma once

#include "GameObjectManager/GameObjectManager.h"

class SpaceSpriteRenderer : public BaseGameObject {
public:

	SpaceSpriteRenderer();
	~SpaceSpriteRenderer();

	void Initialize() override;
	void Update()     override;

private:
	class SpriteRenderer* spriteRenderer_ = nullptr;
};
