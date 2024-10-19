#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
//std
#include<memory>
#include <array>
//obj 
#include"Objects/Tornado/Tornado.h"


class InTornadoEnemy : public BaseGameObject {
public:

	InTornadoEnemy() { CreateTag(this); }
	~InTornadoEnemy() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;
	//getter
	bool GetIsDeath()const { return isDeath_; }
	void OnCollisionEnter([[maybe_unused]] BaseGameObject* const collision);
	//setter
	void SetTornado(Tornado* tornade);
	void SetIsDeath(bool is);

private:

	bool isDeath_;
	float ofsetX;
	float ofsetY;
	float speed_;
	Vec2 radius_;
	//
	float theta_;
	//
	float phi_;

	Tornado* pTornado_ = nullptr;
};