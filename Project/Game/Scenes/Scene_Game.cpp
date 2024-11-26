#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "GraphicManager/Light/DirectionalLight.h"

#include "Objects/Background/BackgroundObjectManager/BackgroundObjectManager.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyCollider/EnemyAttackCollider.h"
#include "Objects/Player/Player.h"
#include "Objects/PlayerHPRenderer/PlayerHPRenderer.h"
#include "Objects/EnemyHPRenderer/EnemyHPRenderer.h"
#include "Objects/EnemyNameRenderer/EnemyNameRenderer.h"
#include "Objects/TrackingCamera/TrackingCamera.h"

#include "Objects/UIManager/UIManager.h"
#include "Objects/GameManagerObject/GameManagerObject.h"
#include "Objects/ModelPreviewObject/ModelPreviewObject.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize(){


	/// モデル確認用のオブジェクト
	(new ModelPreviewObject("Effect6"))->Initialize();
	//(new ModelPreviewObject("Effect3"))->Initialize();


	/// object creata

	/// プレイヤー
	Player* player           = new Player();
	PlayerHPRenderer* playerHPRenderer = new PlayerHPRenderer();

	/// 敵
	EnemyAttackCollider* enemyAttackCollider = new EnemyAttackCollider();
	Enemy* enemy = new Enemy(player,enemyAttackCollider);
	EnemyHPRenderer* enemyHPRenderer = new EnemyHPRenderer();
	EnemyNameRenderer* enemyNameRenderer = new EnemyNameRenderer();

	enemyAttackCollider->SetEnemy(enemy);
	enemyHPRenderer->SetEnemy(enemy);

	BackgroundObjectManager* bbObjectManager = new BackgroundObjectManager();
	UIManager* uiManager = new UIManager();

	TrackingCamera* trackingCamera   = new TrackingCamera(mainCamera_, player, enemy);

	gameManager_ = new GameManagerObject();


	/// 初期化する
	player->SetTrackingCamera(trackingCamera);
	player->SetEnemy(enemy);


	player->Initialize();
	playerHPRenderer->Initialize();
	trackingCamera->Initialize();
	enemy->Initialize();
	enemyHPRenderer->Initialize();
	enemyNameRenderer->Initialize();
	enemyAttackCollider->Initialize();

	bbObjectManager->Initialize();
	uiManager->Initialize();
	uiManager->drawLayerId = GAME_SCENE_LAYER_UI;
	gameManager_->Initialize();

	playerHPRenderer->SetPlayer(player);


	gameManager_->SetPlayer(player);
	gameManager_->SetEnemy(enemy);

	directionalLight_->SetDirection({0.0,-1.0f,0.0f});


	/// ui layer  index=1

	AddLayer("numberLayer", mainCamera_);


	GameCamera* uiCamera = new GameCamera("uiCamera");
	uiCamera->Initialize();
	uiCamera->SetDistance(10.0f);
	uiCamera->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
	AddLayer("ui",uiCamera);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update(){

	if(Input::TriggerKey(KeyCode::Escape)){
		SceneManager::GetInstance()->SetNextScene(RESULT);
	}


	if(gameManager_->GetFlag("isGameClear").Trigger()) {
		SceneManager::GetInstance()->SetNextScene(RESULT);
	}

}
