#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include <ImGuiManager/ImGuiManager.h>

/// components
#include <Component/MeshRenderer/MeshRenderer.h>
#include "Particle/ParticleSystem.h"

/// objects
#include "GameCamera.h"
#include "GameCamera.h"
#include "Ground/Ground.h"
#include "GameCameraState/GameCameraState.h"
#include "Tornado/Tornado.h"	


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	player_                           = new Player;
	buildingManager_                  = new BuildingManager();
	Ground* ground                    = new Ground;
	GameCameraState* gameCameraState  = new GameCameraState();
	Tornado* tornado                  = new Tornado();


	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	tornado->Initialize();

	
	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player_);
	gameCameraState->SetDirectionalLight(directionalLight_);

	//ビル生成
	buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 8.0f, std::numbers::pi_v<float> / 2.0f);
	buildingManager_->SpownBuilding(0, 0);

	tornado->SetPlayer(player_);

}

/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	buildingManager_->Update(player_);

}
