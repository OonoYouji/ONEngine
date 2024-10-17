#include "Scene_Tutorial.h"

//std
#include<numbers>

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Ground/Ground.h"
#include "Objects/GameCameraState/GameCameraState.h"
#include "Objects/Tornado/Tornado.h"	

void Scene_Tutorial::Initialize() {

	Ring::ResetInstanceCount();
	Wind::ResetInstanceCount();

	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	Player* player = new Player;
	buildingManager_ = new BuildingManager();
	Ground* ground = new Ground;
	GameCameraState* gameCameraState = new GameCameraState();
	tornado_ = new Tornado();

	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	tornado_->Initialize();
	buildingManager_->Initialize();

	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player);
	gameCameraState->SetDirectionalLight(directionalLight_);

	tornado_->SetPlayer(player);

	////ビル生成
	//buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 8.0f, std::numbers::pi_v<float> / 2.0f);
	//buildingManager_->SpownBuilding(9, 9);
	//buildingManager_->SpownBuilding(2, 9);
	//buildingManager_->SpownBuilding(9, 4);
	//buildingManager_->SpownBuilding(9, 0);

}
/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Tutorial::Update() {


}
