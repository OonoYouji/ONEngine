#include"BuildingManager.h"

void BuildingManager::Initialize() {

}

//ビル生成
void BuildingManager::SpownBuilding(float theta, float phi) {

	Building* building = new Building();
	building->Initialize();
	// 回転を適用
	Quaternion rotateX = Quaternion::MakeFromAxis({ 1.0f, 0.0f, 0.0f }, phi);
	Quaternion rotateY = Quaternion::MakeFromAxis({ 0.0f, 1.0f, 0.0f }, theta);
	building->SetPivotQuaternion(rotateX * rotateY);

	buildings_.push_back(building);
}

//更新
void 	BuildingManager::Update() {
	for (BaseBuilding* building : buildings_) {
		building->Update();
	}
	buildings_.remove_if([](BaseBuilding* building) {
		if (building->GetIsDeath()) {
			return true;
		}
		return false;
	});
}

void BuildingManager::Debug() {

}

//プレイヤーとペアレント
void  BuildingManager::BehaviorManagement(Tornado* tornado) {
	for (BaseBuilding* building : buildings_) {

		building->BehaviorManagement(tornado);
	}
}

