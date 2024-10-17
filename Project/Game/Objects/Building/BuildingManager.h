#pragma once 
#include<list>

#include"Building.h"
#include"Objects/Building/InTornadoBuilding.h"
#include"Objects/Building/InBossBuilding.h"
#include <ComponentManager/Transform/Transform.h>

#include"Objects/Tornado/Tornado.h"
#include"Objects/Boss/BossBulletLump.h"

class Tornado;
class Boss;
class BuildingManager:public BaseGameObject {
private:
	struct DeathParamater {
		float phi;
		float theta;
		float coolTime;
	};
private:
	Tornado* pTornado_;
	Boss* pBoss_;
	//建物
	std::list<BaseBuilding*> buildings_;
	//巻きこまれてる建物
	std::list<InTornadoBuilding*> inTornadoBuildings_;
	std::list<InBossBuilding*> inBossBuildings_;
	//死亡リスト
	std::list<DeathParamater>deathlist_;
	const float reSpownCoolTime_=4.0f;
	
public:
	
	BuildingManager() { CreateTag(this); }
	~BuildingManager() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;
	//チュートリアル用更新
	void UpdateForTutorial();

	//セット
	void SetBoss(Boss*boss);
	void SetTornado(Tornado*tornado);
	//生成関数
	void SpownBuilding(float theta, float phi);
	void AddInTornadoBuilding(Tornado* tornado, Model* model, BaseBuilding* destBuilding);
	void AddBossBuilding(Boss* boss, Model* model);

	//getter
	std::list<BaseBuilding*> GetBuildings()const { return buildings_; }
	std::list<InBossBuilding*> GetInBossBuilding()const { return inBossBuildings_; }
	size_t GetSize()const { return buildings_.size(); }
	
};
