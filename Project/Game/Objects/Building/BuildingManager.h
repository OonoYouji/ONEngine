#pragma once 
#include<list>

#include"Building.h"
#include"Objects/Building/InTornadoBuilding.h"
#include"Objects/Building/InBossBuilding.h"
#include <ComponentManager/Transform/Transform.h>

#include"Objects/Tornado/Tornado.h"
#include"Objects/Boss/BossBulletLump.h"
class BuildingManager:public BaseGameObject {
private:
	struct DeathParamater {
		float phi;
		float theta;
		float coolTime;
	};
private:

	std::list<BaseBuilding*> buildings_;
	std::list<InTornadoBuilding*> inTornadoBuildings_;
	std::list<InBossBuilding*> inBossBuildings_;
	//死亡リスト
	std::list<DeathParamater>deathlist_;
	const float deathCoolTime_=4.0f;
	
public:
	
	BuildingManager() { CreateTag(this); }
	~BuildingManager() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;

	void AllUpdate(Tornado* tornado,Boss*boss);

	void SpownBuilding(float theta, float phi);
	void AddInTornadoBuilding(Tornado* tornado, Model* model);
	void AddBossBuilding(Boss* boss, Model* model);

	//getter
	std::list<BaseBuilding*> GetBuildings()const { return buildings_; }
	std::list<InBossBuilding*> GetInBossBuilding()const { return inBossBuildings_; }
	/*void 	Draw();*/
};
