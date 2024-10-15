#pragma once

#include "GameObjectManager/GameObjectManager.h"

#include <ComponentManager/AudioSource/AudioSource.h>
//std
#include<memory>
//obj 
#include"Objects/Tornado/Tornado.h"

class InTornadoBuilding : public BaseGameObject {
public:

	InTornadoBuilding() { CreateTag(this); }
	~InTornadoBuilding() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;
	//getter
	bool GetIsDeath()const { return isDeath_; }
	//setter
	void SetModel(Model* model);
	void SetTornado(Tornado* tornade);

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

	Tornado* pTornado_=nullptr;
	
};