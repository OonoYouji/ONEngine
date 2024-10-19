#pragma once 
#include<list>
//obj
#include"Objects/Enemy/Enemy.h"
#include"Objects/Enemy/InTornadoEnemy.h"

#include <ComponentManager/Transform/Transform.h>

//json
#include <Externals/nlohmann/json.hpp>
class Tornado;
class Player;
class EnemyManager :public BaseGameObject {
private:
	struct DeathParamater {
		float phi;
		float theta;
		float coolTime;
	};
private:

	using json = nlohmann::json;
	Tornado* pTornado_;
	Player* pPlayer_;
	//建物
	std::list<Enemy*> enemies_;
	std::list<InTornadoEnemy*> inTornadoEnemies_;
	//死亡リスト
	std::list<DeathParamater>deathlist_;
	const float reSpownCoolTime_ = 4.0f;
	//jSon保存用
	std::vector<std::pair<float, float>>enemyPositions_;

	//デバフ割合
	const float debufPar_ = 1.5f;

public:

	EnemyManager() { CreateTag(this); }
	~EnemyManager() {}

	void Initialize()override;
	void Update() override;
	void Debug() override;
	//チュートリアル用更新
	void UpdateForTutorial();
	void  AddInTornadoBuilding(Tornado* tornado);
	//json
	void SaveEnemyPos(const std::string& filename);
	void LoadEnemyPos(const std::string& filename);

	//セット
	void SetPlayer(Player* player);
	void SetTornado(Tornado* tornado);
	//生成関数
	void SpownEnemy(float theta, float phi);

	float GetDebufParamater();

	//指定の数分ですフラグを立てる
	void SetDeathFlagInBuildings(size_t count);

	//getter
	size_t GetSize()const { return enemies_.size(); }

};
