#pragma once 
#include<list>

#include"Building.h"
#include"Player/Player.h"
#include <Component/Transform/Transform.h>

class BuildingManager:public BaseGameObject {
private:

	std::list<BaseBuilding*> buildings_;
	Vec3 spownPos_;
	Vec3 basePos_;

public:
	
	BuildingManager() { CreateTag(this); }
	~BuildingManager() {}

	void Initialize()override;
	void Update() override;

	void Debug() override;

	void SpownBuilding(float theta, float phi);
	//void 	Init();
	void 	RemoveBuilding();
	void 	BehaviorManagement(Player*player);
	/*void 	Draw();*/
};
