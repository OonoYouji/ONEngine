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
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyManager.h"
#include "Objects/SkyDome/SkyDome.h"
#include "Objects/DefeatedEnemy/DefeatedEnemy.h"
#include "Objects/ScoreBoard/ScoreBoard.h"
#include "Objects/Score/Score.h"
#include "Objects/BulletFiringEnergyRenderer/BulletFiringEnergyRenderer.h"
#include "Objects/Terrain/Terrain.h"

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
	RailCamera*     railCamera     = new RailCamera();
	Reticle*        reticle        = new Reticle();
	Player*         player         = new Player();
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
	railCamera->Initialize();
	reticle->Initialize();
	player->Initialize();
	enemyManager->Initialize();
	skyDome->Initialize();
	defeatedEnemy->Initialize();
	scoreBoard->Initialize();
	score->Initialize();
	bulletFiringEnergyRenderer->Initialize();
	terrain->Initialize();

	/// その他ポインタ設定など...

	gameManager_->SetRailCamera(railCamera);

	railCamera->SetGameCamera(mainCamera_);
	railCamera->SetShootingCourse(shootingCourse);
	railCamera->SetDirectionalLight(directionalLight_);

	player->SetParent(railCamera->GetTransform());
	player->SetReticle(reticle);
	player->SetScoreObj(score);
	player->SetDefeatedEnemy(defeatedEnemy);

	reticle->SetGameCamera(mainCamera_);

	enemyManager->SetShootingCourse(shootingCourse);
	enemyManager->SetRailCamera(railCamera);

	skyDome->SetOffsetObject(railCamera);

	bulletFiringEnergyRenderer->SetPlayer(player);

	mainCamera_->SetPosition({ 0.0f, 1.0f, 0.0f }); /// レールに被らないように少し上に設定


	/// add layers

	GameCamera* defeatedEnemyCamera = new GameCamera("defeatedEnemyCamera");
	defeatedEnemyCamera->Initialize();
	
	GameCamera* uiCamera = new GameCamera("uiCamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);
	uiCamera->SetDistance(10.0f);

	AddLayer("defeatedEnemyLayer", defeatedEnemyCamera);
	AddLayer("uiLayer", uiCamera);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	

	/// ゲームが終了したのでシーン遷移
	if(gameManager_->GetIsGameEnd()) {
		
		/*##########################################################
			TODO : COMMENT
			シーン遷移を追加する -> 暗転
		##########################################################*/
		SceneManager::GetInstance()->SetNextScene(RESULT);
	}

}
