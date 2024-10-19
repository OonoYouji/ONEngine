#include"EnemyManager.h"
#include"FrameManager/Time.h"
#include"Math/Random.h"
//std
#include<numbers>
#include<string>
#include <fstream>
#include"ImGuiManager/ImGuiManager.h"

void EnemyManager::Initialize() {

}

//ビル生成
void EnemyManager::SpownEnemy(float phi, float theta) {

	Enemy* enemy = new Enemy();
	enemy->Initialize();
	enemy->SetPlayer(pPlayer_);
	//死亡パラメータ取得
	enemy->SetPos(std::pair<float, float>(phi, theta));
	// 回転を適用
	Quaternion rotateX = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, phi);
	Quaternion rotateY = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, theta);
	enemy->SetPivotQuaternion(rotateX * rotateY);
	enemyPositions_.push_back(std::pair<float, float>(phi, theta));
	enemies_.push_back(enemy);
}
//巻きこまれてるビルの追加
void  EnemyManager::AddInTornadoBuilding(Tornado* tornado) {
	InTornadoEnemy* inTornadoEnemy = new InTornadoEnemy();
	inTornadoEnemy->Initialize();
	//トルネードをセット
	inTornadoEnemy->SetTornado(tornado);
	//リストに追加
	inTornadoEnemies_.push_back(inTornadoEnemy);
}

//更新
void 	EnemyManager::Update() {

	auto posIte = enemyPositions_.begin();
	// 建ってるビル達の更新
	for (std::list<Enemy*>::iterator enemyIter = enemies_.begin(); enemyIter != enemies_.end(); ) {
		//更新
		
		if (posIte != enemyPositions_.end()) {
			*posIte = (*enemyIter)->GetPos(); // 更新された座標を入れる
			++posIte; // 次の要素へ進める
		}
		//死亡
		if ((*enemyIter)->GetIsInTornado()) {

			//巻きこまれるビルを追加
			AddInTornadoBuilding(pTornado_);

			//死亡パラメータを取得してリストに追加
			DeathParamater deathParamager;
			deathParamager.phi = (*enemyIter)->GetPos().first + Random::Float(-0.5f, 0.5f);
			deathParamager.theta = (*enemyIter)->GetPos().second + Random::Float(-0.5f, 0.5f);
			deathParamager.coolTime = reSpownCoolTime_;
			deathlist_.push_back(deathParamager);
			//デストロイ
			(*enemyIter)->Destory();
			// リストから削除	
			enemyIter = enemies_.erase(enemyIter);
		}
		else {
			++enemyIter;
		}
	}

	// 巻きこまれてるビル達の更新
	for (auto enemyIner = inTornadoEnemies_.begin(); enemyIner != inTornadoEnemies_.end(); ) {

		(*enemyIner)->Update();
		//
		if ((*enemyIner)->GetIsDeath()) {
			(*enemyIner)->Destory();

			enemyIner = inTornadoEnemies_.erase(enemyIner); // リストから削除	
		}
		else {
			++enemyIner;
		}
	}

	// 死んだ建物の復活
	for (std::list<DeathParamater>::iterator deathListiter = deathlist_.begin(); deathListiter != deathlist_.end(); ) {

		//更新
		(*deathListiter).coolTime -= Time::DeltaTime();
		//クールタイムが終わったら再スポーン
		if ((*deathListiter).coolTime <= 0) {
			SpownEnemy((*deathListiter).phi, (*deathListiter).theta);
			deathListiter = deathlist_.erase(deathListiter);
		}
		else {
			++deathListiter;
		}
	}
}

void EnemyManager::Debug() {
	if (ImGui::Button("Save")) {
		SaveEnemyPos("resources/EnemyParamater/EnemyPos.json");
		std::string message = std::format("{}.json saved.", "Enemy");
		MessageBoxA(nullptr, message.c_str(), "EnemyParamater", 0);
	}

	if (ImGui::Button("AddEnemy")) {
		SpownEnemy(9, 4);
	}
}


void EnemyManager::SetPlayer(Player* player) {
	pPlayer_ = player;
}
void EnemyManager::SetTornado(Tornado* tornado) {
	pTornado_ = tornado;
}

// レールの制御点をJSON形式で保存する
void EnemyManager::SaveEnemyPos(const std::string& filename) {
	json j;
	for (const auto& pos : enemyPositions_) {
		j.push_back({ pos.first,pos.second });
	}

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(4);  // 4はインデント幅
		file.close();
	}
}

// JSONファイルから制御点を読み込む
void EnemyManager::LoadEnemyPos(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			std::pair<float, float> pos = { spot[0], spot[1] };
			SpownEnemy(pos.first, pos.second);
		}
	}
}