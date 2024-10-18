#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include <ImGuiManager/ImGuiManager.h>

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>


/// objects
#include "Objects/Camera/GameCamera.h"

#include "Objects/GameManager/GameManager.h"
#include "Objects/Ground/Ground.h"
#include "Objects/Boss/Boss.h"
#include "Objects/Boss/BossVacuum.h"
#include "Objects/CameraState/GameCameraState.h"
#include "Objects/CameraState/CameraStateBossEntryToGame.h"
#include "Objects/Tornado/Tornado.h"


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	Ring::ResetInstanceCount();
	Wind::ResetInstanceCount();


	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	player_            = new Player;
	buildingManager_   = new BuildingManager();
	boss_              = new Boss();
	tornado_           = new Tornado();
	BossTubu* bossTube = new BossTubu();
	BossHead* bossHead = new BossHead();
	Ground* ground = new Ground;
	GameCameraState* gameCameraState = new GameCameraState();
	/*GameCameraZoomInOut* gameCameraZoomInOut = new GameCameraZoomInOut();*/
	tornado_ = new Tornado();


	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
	boss_->Initialize();
	bossTube->Initialize();
	bossHead->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	tornado_->Initialize();
	buildingManager_->Initialize();

	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player_);
	gameCameraState->SetDirectionalLight(directionalLight_);

	player_->SetBuildingManager(buildingManager_);
	player_->SetTornado(tornado_);

	boss_->SetPlayer(player_);
	boss_->SetBuildingaManager(buildingManager_);
	boss_->SetHead(bossHead);
	boss_->SetTubu(bossTube);

	bossTube->SetBoss(boss_);
	bossHead->SetBossTube(bossTube);

	tornado_->SetPlayer(player_);
	
	buildingManager_->SetBoss(boss_);
	buildingManager_->SetTornado(tornado_);

	//jsonからデータ読む
	buildingManager_->LoadControlSpots("resources/BuildingParamater/BuildingPos.json");
	//ビル生成
	/*buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 8.0f, std::numbers::pi_v<float> / 2.0f);
	buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 5.0f, std::numbers::pi_v<float> / 9.0f);
	buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 2, std::numbers::pi_v<float> / 9);
	buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 9, std::numbers::pi_v<float> / 4);
	buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 4, std::numbers::pi_v<float> / 3);*/
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	


}
