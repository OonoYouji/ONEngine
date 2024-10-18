#include"BuildingManager.h"
#include"FrameManager/Time.h"
//std
#include<numbers>
#include<string>
#include <fstream>
#include"ImGuiManager/ImGuiManager.h"

void BuildingManager::Initialize() {
	
}

//ビル生成
void BuildingManager::SpownBuilding(float phi, float theta) {

	Building* building = new Building();
	building->Initialize();
	building->SetBoss(pBoss_);
	//死亡パラメータ取得
	building->SetPos(std::pair<float,float>(phi, theta));
	// 回転を適用
	Quaternion rotateX = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f },  theta);
	Quaternion rotateY = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f },  phi);
	buildingPositions_.push_back(std::pair<float, float>(phi, theta));
	building->SetPivotQuaternion(rotateX * rotateY);

	buildings_.push_back(building);
}
//巻きこまれてるビルの追加
void  BuildingManager::AddInTornadoBuilding(Tornado* tornado, Model* model) {
	InTornadoBuilding* inTornadoBuilding = new InTornadoBuilding();
	inTornadoBuilding->Initialize();
	//モデルセット
	inTornadoBuilding->SetModel(model);
	//トルネードをセット
	inTornadoBuilding->SetTornado(tornado);
	//リストに追加
	inTornadoBuildings_.push_back(inTornadoBuilding);
}

//ボスが持つするビルの追加
void  BuildingManager::AddBossBuilding(Boss* boss, Model* model) {
	InBossBuilding* inbossBuilding = new InBossBuilding();
	inbossBuilding->Initialize();
	//モデルセット
	inbossBuilding->SetModel(model);
	//ボスセット
	inbossBuilding->SetBoss(boss);
	//リストに追加
	inBossBuildings_.push_back(inbossBuilding);
}

//更新
void 	BuildingManager::Update() {
	
	auto posIte = buildingPositions_.begin();
	// 建ってるビル達の更新
	for (std::list<BaseBuilding*>::iterator buildingIter = buildings_.begin(); buildingIter != buildings_.end(); ) {
		//更新
		(*buildingIter)->Update();
		if (posIte != buildingPositions_.end()) {
			*posIte = (*buildingIter)->GetPos(); // 更新された座標を入れる
			++posIte; // 次の要素へ進める
		}

		//死亡
		if ((*buildingIter)->GetIsInTornado() || (*buildingIter)->GetIsTaken() || (*buildingIter)->GetIsBreak()) {
			if ((*buildingIter)->GetIsInTornado()) {//竜巻による場合
				//巻きこまれるビルを追加
				AddInTornadoBuilding(pTornado_, (*buildingIter)->GetModel());
			}
			else if ((*buildingIter)->GetIsTaken()) {//ボスによる場合
				AddBossBuilding(pBoss_, (*buildingIter)->GetModel());//ボス用建物追加
				pBoss_->SetIsSlurping(false);
				pBoss_->SetSlurpingCoolTimer();
			}
			//死亡パラメータを取得してリストに追加
			DeathParamater deathParamager;
			deathParamager.phi = (*buildingIter)->GetPos().first;
			deathParamager.theta = (*buildingIter)->GetPos().second;
			deathParamager.coolTime = reSpownCoolTime_;
			deathlist_.push_back(deathParamager);
			//デストロイ
			(*buildingIter)->Destory();
			// リストから削除	
			buildingIter = buildings_.erase(buildingIter);
		}
		else {
			++buildingIter;
		}
	}

	// 巻きこまれてるビル達の更新
	for (auto buildingIter = inTornadoBuildings_.begin(); buildingIter != inTornadoBuildings_.end(); ) {

		(*buildingIter)->Update();
		//
		if ((*buildingIter)->GetIsDeath()) {
			(*buildingIter)->Destory();

			buildingIter = inTornadoBuildings_.erase(buildingIter); // リストから削除	
		}
		else {
			++buildingIter;
		}
	}

	// ボスのビル達の更新
	for (auto buildingIter = inBossBuildings_.begin(); buildingIter != inBossBuildings_.end(); ) {

		(*buildingIter)->Update();
		//
		if ((*buildingIter)->GetIsDeath()) {
			(*buildingIter)->Destory();

			buildingIter = inBossBuildings_.erase(buildingIter); // リストから削除	
		}
		else {
			++buildingIter;
		}
	}

	// 死んだ建物の復活
	for (std::list<DeathParamater>::iterator deathListiter = deathlist_.begin(); deathListiter != deathlist_.end(); ) {

		//更新
		(*deathListiter).coolTime -= Time::DeltaTime();
		//クールタイムが終わったら再スポーン
		if ((*deathListiter).coolTime <= 0) {
			SpownBuilding((*deathListiter).phi, (*deathListiter).theta);
			deathListiter = deathlist_.erase(deathListiter);
		}
		else {
			++deathListiter;
		}
	}
}
//指定の数分死亡フラグを立てる処理
void BuildingManager::SetDeathFlagInBuildings(size_t count) {
	size_t currentCount = 0;

	// 建物リストをループしてデスフラグを立てる
	for (auto& building : inTornadoBuildings_) {
		if (currentCount >= count) {
			break; // 指定数に達したらループを終了
		}

		// デスフラグを立てる処理
		building->SetIsDeath(true); 
		currentCount++;
	}
}

void BuildingManager::Debug() {
	if (ImGui::Button("Save")) {
		SaveBuildingPos("resources/BuildingParamater/BuildingPos.json");
		std::string message = std::format("{}.json saved.", "Building");
		MessageBoxA(nullptr, message.c_str(), "BuildingParamater", 0);
	}

	if (ImGui::Button("AddBuilding")) {
		SpownBuilding(9,4);
	}
}


void BuildingManager::SetBoss(Boss* boss) {
	pBoss_ = boss;
}
void BuildingManager::SetTornado(Tornado* tornado) {
	pTornado_ = tornado;
}

//チューとるリアル用更新
void BuildingManager::UpdateForTutorial() {
	// 建ってるビル達の更新
	for (std::list<BaseBuilding*>::iterator buildingIter = buildings_.begin(); buildingIter != buildings_.end(); ) {
		//更新
		(*buildingIter)->Update();
		//死亡
		if ((*buildingIter)->GetIsInTornado()) {
			if ((*buildingIter)->GetIsInTornado()) {//竜巻による場合
				//巻きこまれるビルを追加
				AddInTornadoBuilding(pTornado_, (*buildingIter)->GetModel());
				//デストロイ
				(*buildingIter)->Destory();
				// リストから削除	
				buildingIter = buildings_.erase(buildingIter);
			}
			
		}
		else {
			++buildingIter;
		}
	}
}


// レールの制御点をJSON形式で保存する
void BuildingManager::SaveBuildingPos(const std::string& filename) {
	json j;
	for (const auto& pos : buildingPositions_) {
		j.push_back({ pos.first,pos.second});
	}

	std::ofstream file(filename);
	if (file.is_open()) {
		file << j.dump(4);  // 4はインデント幅
		file.close();
	}
}

// JSONファイルから制御点を読み込む
void BuildingManager::LoadControlSpots(const std::string& filename) {
	std::ifstream file(filename);
	if (file.is_open()) {
		json j;
		file >> j;
		file.close();

		// 読み込んだデータを基にレールを追加
		for (const auto& spot : j) {
			std::pair<float,float> pos = { spot[0], spot[1]};
			SpownBuilding(pos.first,pos.second);
		}
	}
}