#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
//std
#include<memory>
//obj 
#include"Objects/Boss/Boss.h"

class InBossBuilding : public BaseGameObject {
public:

	InBossBuilding() { CreateTag(this); }
	~InBossBuilding() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;
	//getter
	bool GetIsDeath()const { return isDeath_; }
	//setter
	void SetModel(Model* model);
	void SetBoss(Boss* tornade);

private:
	bool isDeath_;
	float ofsetX;
	float ofsetY;
	float speed_;
	float radius_;
	//
	float theta_;
	//
	float phi_;

	Boss* pBoss_;
};