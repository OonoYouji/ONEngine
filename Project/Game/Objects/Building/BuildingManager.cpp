#include"BuildingManager.h"
#include"FrameManager/Time.h"

void BuildingManager::Initialize() {

}

//ビル生成
void BuildingManager::SpownBuilding(float theta, float phi) {

	Building* building = new Building();
	building->Initialize();
	//死亡パラメータ取得
	building->SetPhi(theta);
	building->SetPhi(phi);
	// 回転を適用
	Quaternion rotateX = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, phi);
	Quaternion rotateY = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, theta);
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

	// 建ってるビル達の更新
	for (std::list<BaseBuilding*>::iterator buildingIter = buildings_.begin(); buildingIter != buildings_.end(); ) {
		//更新
		(*buildingIter)->Update();
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
			deathParamager.phi = (*buildingIter)->GetPhi();
			deathParamager.theta = (*buildingIter)->GetTheta();
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
			SpownBuilding((*deathListiter).theta, (*deathListiter).phi);
			deathListiter = deathlist_.erase(deathListiter);
		}
		else {
			++deathListiter;
		}
	}
}


void BuildingManager::Debug() {

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