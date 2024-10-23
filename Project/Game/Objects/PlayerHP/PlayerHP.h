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
	Vec2   gaugeUVPosition_;

	Vec2   numberUVPosition_;
	Vec2   numberUVScale_;

	class MeshRenderer* numberRenderer_ = nullptr;
	
};
