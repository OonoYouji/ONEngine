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
#include "Objects/GameCameraState/GameCameraState.h"
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
	Ground* ground = new Ground;
	GameCameraState* gameCameraState = new GameCameraState();
	tornado_ = new Tornado();


	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
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

	//ビル生成
	buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 8.0f, std::numbers::pi_v<float> / 2.0f);
	buildingManager_->SpownBuilding(9, 9);
	buildingManager_->SpownBuilding(2, 9);
	buildingManager_->SpownBuilding(9, 4);
	buildingManager_->SpownBuilding(9, 0);

	tornado_->SetPlayer(player_);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	//ビルの振る舞い管理
	buildingManager_->AllUpdate(tornado_);
	//プレイヤーのゲージMaxでカメラズームアウト
	if (player_->GetisPowerUp()) {
		mainCamera_->SetBehaviorZoomOut();
	}
	//ズームイン
	else if (!player_->GetisPowerUp()) {
		mainCamera_->SetBehaviorZoomIn();
	}

}