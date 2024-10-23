#pragma once

#include "GameObjectManager/GameObjectManager.h"

class Enemy : public BaseGameObject {
public:

	Enemy();
	~Enemy();

	void Initialize() override;
	void Update()     override;

private:
	class MeshRenderer* meshRenderer_ = nullptr;
};
