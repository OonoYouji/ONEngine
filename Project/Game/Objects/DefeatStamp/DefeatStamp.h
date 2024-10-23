#pragma once

#include "GameObjectManager/GameObjectManager.h"



class DefeatStamp : public BaseGameObject {
public:

	DefeatStamp();
	~DefeatStamp();

	void Initialize() override;
	void Update()     override;

private:
	class SpriteRenderer* spriteRenderer_ = nullptr;
	class Stamp* stamp_ = nullptr;
};



class Stamp : public BaseGameObject {
public:

	Stamp();
	~Stamp();

	void Initialize() override;
	void Update()     override;
	void Debug()      override;

private:
	class SpriteRenderer* spriteRenderer_ = nullptr;

	float waitTime_ = 0.25f;
	float animationTime_    = 0.0f;
	float maxAnimationTime_ = 1.0f;
};

