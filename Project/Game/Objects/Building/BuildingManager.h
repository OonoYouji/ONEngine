#pragma once 
#include<list>

#include"Building.h"
#include"Objects/Tornado/Tornado.h"
#include <ComponentManager/Transform/Transform.h>

class BuildingManager:public BaseGameObject {
private:

	std::list<BaseBuilding*> buildings_;
	Vec3 spownPos_;
	Vec3 basePos_;
	////トルネード
	//Tornado* pTornado_;

public:
	
	BuildingManager() { CreateTag(this); }
	~BuildingManager() {}

	void Initialize()override;
	void Update() override;

	void Debug() override;

	void SpownBuilding(float theta, float phi);
	//void 	Init();
	void 	RemoveBuilding();
	void 	BehaviorManagement(Tornado*player);
	/*void 	Draw();*/
};
