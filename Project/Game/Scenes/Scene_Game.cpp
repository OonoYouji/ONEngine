#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include <ImGuiManager/ImGuiManager.h>
#include "Scenes/Manager/SceneManager.h"

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
#include "Objects/Enemy/EnemyManager.h"
#include "Objects/PlayerHP/PlayerHP.h"
#include "Objects/SceneTransition/SceneTransition.h"
#include "Objects/DeadEffect/DeadEffect.h"
#include "Objects/GameResult/GameResult.h"
#include "Objects/PowerUpGauge/PowerUpGauge.h"

/// ui
#include "UI/GuidanceArrow.h"

int Scene_Game::sGameResult_ = false;


/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	sGameResult_ = false;

	Wind::ResetInstanceCount();


	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	player_            = new Player;
	PlayerHP* playerHP = new PlayerHP;
	buildingManager_   = new BuildingManager();
	boss_              = new Boss();
	tornado_           = new Tornado();
	/*EnemyManager* enemyManager = new EnemyManager();*/
	BossTubu* bossTube = new BossTubu();
	BossHead* bossHead = new BossHead();
	Ground* ground = new Ground;
	GameCameraState* gameCameraState = new GameCameraState();
	/*GameCameraZoomInOut* gameCameraZoomInOut = new GameCameraZoomInOut();*/
	GuidanceArrow* guideArrow = new GuidanceArrow();
	PowerUpGauge* powerUpGauge = new PowerUpGauge();

	sceneTransition_ = new SceneTransition(TRANSTION_FADE_IN);
	deadEffect_ = new DeadEffect();

	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
	playerHP->Initialize();
	boss_->Initialize();
	//enemyManager->Initialize();
	bossTube->Initialize();
	bossHead->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	tornado_->Initialize();
	buildingManager_->Initialize();
	guideArrow->Initialize();
	sceneTransition_->Initialize();
	deadEffect_->Initialize();
	powerUpGauge->Initialize();

	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player_);
	gameCameraState->SetDirectionalLight(directionalLight_);

	player_->SetBuildingManager(buildingManager_);
	player_->SetTornado(tornado_);
	player_->SetPlayerHP(playerHP);

	boss_->SetPlayer(player_);
	boss_->SetBuildingaManager(buildingManager_);
	boss_->SetHead(bossHead);
	boss_->SetTubu(bossTube);

	bossTube->SetBoss(boss_);
	bossHead->SetBossTube(bossTube);

	tornado_->SetPlayer(player_);
	
	buildingManager_->SetBoss(boss_);
	buildingManager_->SetTornado(tornado_);

	guideArrow->SetBoss(boss_);
	guideArrow->SetPlayer(player_);

	//jsonからデータ読む
	/*enemyManager->LoadEnemyPos("resources/EnemyParamater/EnemyPos.json");*/
	buildingManager_->LoadBuildingPos("resources/BuildingParamater/BuildingPos.json");



	/// layer

	GameCamera* uiCamera = new GameCamera("uiCamera");
	uiCamera->Initialize();

	AddLayer("ui", uiCamera);
	playerHP->drawLayerId = 1;
	
	AddLayer("transition", uiCamera);
	sceneTransition_->drawLayerId = 2;



	bgm_.reset(new AudioSource);
	bgm_->Initialize();
	bgm_->SetAudioClip("gameBgm.wav");
	bgm_->isLoop = true;
	bgm_->volume = 0.1f;
	bgm_->PlayAudio();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	
	if(!player_->GetIsAlive()) {
		deadEffect_->SetIsStart(true);
		if(!deadEffect_->GetParent()) {
			deadEffect_->SetParent(player_->GetTransform());
			sGameResult_ = GAME_RESULT_GAME_OVER;
		}
	}

	if(!boss_->GetIsAlive()) {
		deadEffect_->SetIsStart(true);
		if(!deadEffect_->GetParent()) {
			deadEffect_->SetParent(boss_->GetTransform());
			sGameResult_ = GAME_RESULT_CLEAR;
		}
	}

	if(deadEffect_->IsFinished()) {
		sceneTransition_->SetIsStarted(true);
	}

	if(sceneTransition_->GetIsFinished()) {
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::RESULT);
		bgm_->StopAudioAll();
	}

}
