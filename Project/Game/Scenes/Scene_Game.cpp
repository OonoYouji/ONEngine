#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include <ImGuiManager/ImGuiManager.h>

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>


/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Ground/Ground.h"
#include "Objects/Boss/Boss.h"
#include "Objects/GameCameraState/GameCameraState.h"
#include"Objects/GameCameraState/GameCameraZoomInOut.h"
#include "Objects/Tornado/Tornado.h"	


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	player_ = new Player;
	buildingManager_ = new BuildingManager();
	 boss_ = new Boss();
	Ground* ground = new Ground;
	GameCameraState* gameCameraState = new GameCameraState();
	GameCameraZoomInOut* gameCameraZoomInOut = new GameCameraZoomInOut();
	tornado_ = new Tornado();


	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
	boss_->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	gameCameraZoomInOut->Initialize();
	tornado_->Initialize();
	buildingManager_->Initialize();


	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player_);
	gameCameraState->SetDirectionalLight(directionalLight_);

	gameCameraZoomInOut->SetGameCamera(mainCamera_);
	gameCameraZoomInOut->SetPlayer(player_);

	boss_->SetPlayer(player_);
	boss_->SetBuildingaManager(buildingManager_);

	tornado_->SetPlayer(player_);
	//ビル生成
	buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 8.0f, std::numbers::pi_v<float> / 2.0f);
	buildingManager_->SpownBuilding(9, 9);
	buildingManager_->SpownBuilding(2, 9);
	buildingManager_->SpownBuilding(9, 4);
	buildingManager_->SpownBuilding(9, 0);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	//ビルの振る舞い管理
	buildingManager_->AllUpdate(tornado_, boss_);
	

}
