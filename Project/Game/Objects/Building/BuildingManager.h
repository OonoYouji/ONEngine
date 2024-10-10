#pragma once 

/// std
#include <list>

/// objects
#include "Objects/Building/Building.h"
#include "Objects/Player/Player.h"

/// components
#include <ComponentManager/Transform/Transform.h>



class BuildingManager:public BaseGameObject {
private:

	//パーティクルリスト
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
	void 	ParentPlayer(Player*player);
	/*void 	Draw();*/
};
