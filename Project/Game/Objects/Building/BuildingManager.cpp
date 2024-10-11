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

//更新
void 	BuildingManager::Update() {

	// 建ってるビル達の更新
	for (std::list<DeathParamater>::iterator deathListiter = deathlist_.begin(); deathListiter != deathlist_.end(); ) {

		//更新
		(*deathListiter).coolTime -= Time::DeltaTime();
		//クールタイムが終わったら再スポーン
		if ((*deathListiter).coolTime<=0) {
			SpownBuilding((*deathListiter).theta, (*deathListiter).phi);
			deathListiter = deathlist_.erase(deathListiter);
		}
		else {
			++deathListiter;
		}
	}
}

//更新
void 	BuildingManager::AllUpdate(Tornado* tornado) {
	// 建ってるビル達の更新
	for (auto buildingIter = buildings_.begin(); buildingIter != buildings_.end(); ) {
	
		//更新
		(*buildingIter)->Update();

		//死亡
		if ((*buildingIter)->GetIsDeath()) {
			//巻きこまれるビルを追加
			AddInTornadoBuilding(tornado, (*buildingIter)->GetModel());
			//死亡パラメータを取得してリストに追加
			DeathParamater deathParamager;
			deathParamager.phi = (*buildingIter)->GetPhi();
			deathParamager.theta = (*buildingIter)->GetTheta();
			deathParamager.coolTime = deathCoolTime_;
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

	//// 巻きこまれてるビル達の更新
	//for (auto buildingIter = inTornadoBuildings_.begin(); buildingIter != inTornadoBuildings_.end(); ) {

	//	(*buildingIter)->Update();
	//	//
	//	if ((*buildingIter)->GetIsDeath()) {
	//		(*buildingIter)->Destory();

	//		buildingIter = inTornadoBuildings_.erase(buildingIter); // リストから削除	
	//	}
	//	else {
	//		++buildingIter;
	//	}
	//}
}

void BuildingManager::Debug() {

}


