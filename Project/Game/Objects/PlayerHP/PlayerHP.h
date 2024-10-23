#pragma once

#include "GameObjectManager/GameObjectManager.h"

class PlayerHP : public BaseGameObject {
public:

	PlayerHP();
	~PlayerHP();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:
	Model* model_ = nullptr;
	Vec2   uvPosition_;
	
};
