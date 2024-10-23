#pragma once

/// std
#include <vector>

/// base class
#include "GameObjectManager/GameObjectManager.h"

class PlayerHP : public BaseGameObject {
public:

	PlayerHP();
	~PlayerHP();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

	void ResetAnimationTime();

	void SetHP(int hp);

private:
	Model* model_ = nullptr;
	Vec2   gaugeUVPosition_;

	Vec2   numberUVPosition_;
	Vec2   numberUVScale_;

	/// pos yを格納
	std::vector<float> uvPosYArray_;

	/// hp
	int currentHp_;
	int nextHp_;

	/// time
	float animationTime_    = 0.0f;
	float maxAnimationTime_ = 0.0f;

	bool isStart_ = false;

	class MeshRenderer* numberRenderer_ = nullptr;
};
