#pragma once 
#include<list>

#include"Building.h"
#include"Objects/Tornado/Tornado.h"
#include"Objects/Building/InTornadoBuilding.h"
#include <ComponentManager/Transform/Transform.h>

class BuildingManager:public BaseGameObject {
private:

	std::list<BaseBuilding*> buildings_;
	std::list<InTornadoBuilding*> inTornadoBuildings_;
	
public:
	
	BuildingManager() { CreateTag(this); }
	~BuildingManager() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;

	void AllUpdate(Tornado* tornado);

	void SpownBuilding(float theta, float phi);
	void AddInTornadoBuilding(Tornado* tornado, Model* model);
	

	/*void 	Draw();*/
};
