#include "Scene_Game.h"

/// engine
#include "Scenes/Manager/SceneManager.h"

/// component
#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// default objects
#include "Objects/Camera/GameCamera.h"

/// objects
#include "Objects/GameManager/GameManager.h"
#include "Objects/Player/Player.h"
#include "Objects/ShootingCourse/ShootingCourse.h"
#include "Objects/RailCamera/RailCamera.h"
#include "Objects/Reticle/Reticle.h"
#include "Objects/Boss/Boss.h"
#include "Objects/Enemy/EnemyManager.h"
#include "Objects/SkyDome/SkyDome.h"
#include "Objects/DefeatedEnemy/DefeatedEnemy.h"
#include "Objects/ScoreBoard/ScoreBoard.h"
#include "Objects/Score/Score.h"
#include "Objects/BulletFiringEnergyRenderer/BulletFiringEnergyRenderer.h"
#include "Objects/Terrain/Terrain.h"

#include "Objects/Home/Home.h"
#include "Objects/Rock/Rock.h"
#include "Objects/WelcomeBoard/WelcomeBoard.h"

#include "Objects/SceneTransition/SceneTransition.h"
	
/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	/// camera setting
	mainCamera_->SetPosition({});
	mainCamera_->SetRotate({});


	/// instance create...
	gameManager_                   = new GameManager();
	ShootingCourse* shootingCourse = new ShootingCourse();
	railCamera_                    = new RailCamera();
	Reticle*        reticle        = new Reticle();
	Player*         player         = new Player();
	Boss*           boss = new Boss();
	EnemyManager*   enemyManager   = new EnemyManager();
	SkyDome*        skyDome        = new SkyDome();
	DefeatedEnemy*  defeatedEnemy  = new DefeatedEnemy();
	ScoreBoard*     scoreBoard     = new ScoreBoard();
	Score*          score          = new Score();
	BulletFiringEnergyRenderer* bulletFiringEnergyRenderer = new BulletFiringEnergyRenderer();
	Terrain*        terrain        = new Terrain();

	/// instance initializing...
	gameManager_->Initialize();
	shootingCourse->Initialize();
	railCamera_->Initialize();
	reticle->Initialize();
	player->Initialize();
	enemyManager->Initialize();
	boss->Initialize();
	skyDome->Initialize();
	defeatedEnemy->Initialize();
	scoreBoard->Initialize();
	score->Initialize();
	bulletFiringEnergyRenderer->Initialize();
	terrain->Initialize();

	/// bb objects
	(new Home())->Initialize();
	(new Rock())->Initialize();
	(new WelcomeBoard())->Initialize();



	/// その他ポインタ設定など...

	gameManager_->SetRailCamera(railCamera_);

	railCamera_->SetGameCamera(mainCamera_);
	railCamera_->SetShootingCourse(shootingCourse);
	railCamera_->SetDirectionalLight(directionalLight_);

	player->SetParent(railCamera_->GetTransform());
	player->SetReticle(reticle);
	player->SetScoreObj(score);
	player->SetDefeatedEnemy(defeatedEnemy);

	reticle->SetGameCamera(mainCamera_);

	enemyManager->SetShootingCourse(shootingCourse);
	enemyManager->SetRailCamera(railCamera_);

	boss->SetRailCamera(railCamera_);

	skyDome->SetOffsetObject(railCamera_);

	bulletFiringEnergyRenderer->SetPlayer(player);

	mainCamera_->SetPosition({ 0.0f, 1.0f, 0.0f }); /// レールに被らないように少し上に設定


	/// add layers

	GameCamera* defeatedEnemyCamera = new GameCamera("defeatedEnemyCamera");
	defeatedEnemyCamera->Initialize();
	
	GameCamera* uiCamera = new GameCamera("uiCamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);
	uiCamera->SetDistance(10.0f);
	
	GameCamera* transitionCamera = new GameCamera("transitionCamera");
	transitionCamera->Initialize();

	AddLayer("defeatedEnemyLayer", defeatedEnemyCamera);
	AddLayer("uiLayer", uiCamera);
	AddLayer("reticleLayer", mainCamera_);
	AddLayer("transitionLayer", transitionCamera);

	isStarted_ = false;
	sceneTransition_ = nullptr;
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	


	/// ゲームが終了したのでシーン遷移
	if(gameManager_->GetIsGameEnd()) {
		
		if(!sceneTransition_) {
			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 1.0f, SCENE_GAME_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		} else {

			if(sceneTransition_->GetIsEnd()) {

				SceneManager::GetInstance()->SetNextScene(RESULT);
			}

		}

	} else {


		/// ゲーム開始の処理
		if(!isStarted_) {
			isStarted_ = true;

			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_OUT, 2.0f, SCENE_GAME_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		} else {

			if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
				railCamera_->isActive = true;

				sceneTransition_->Destory();
				sceneTransition_ = nullptr;
			}
		}
	}

}
