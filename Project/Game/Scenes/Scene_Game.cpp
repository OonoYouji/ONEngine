#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include <ImGuiManager/ImGuiManager.h>

/// components
#include <ComponentManager/MeshRenderer/MeshRenderer.h>

/// objects
#include "Objects/Camera/GameCamera.h"
#include "GraphicManager/Light/DirectionalLight.h"

#include "Objects/Background/BackgroundObjectManager/BackgroundObjectManager.h"
#include "Objects/Enemy/Enemy.h"
#include "Objects/Enemy/EnemyCollider/EnemyAttackCollider.h"
#include "Objects/Player/Player.h"
#include "Objects/PlayerHPRenderer/PlayerHPRenderer.h"
#include "Objects/TrackingCamera/TrackingCamera.h"

#include "Objects/ModelPreviewObject/ModelPreviewObject.h"

/// lib
#include "Debugger/Assertion.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize(){


	/// モデル確認用のオブジェクト
	(new ModelPreviewObject("Player_Wait"))->Initialize();
	(new ModelPreviewObject("Player_Wait"))->Initialize();


	/// object creata

	/// プレイヤー
	Player* player           = new Player(mainCamera_);
	PlayerHPRenderer* playerHPRenderer = new PlayerHPRenderer();

	/// 敵
	EnemyAttackCollider* enemyAttackCollider = new EnemyAttackCollider();
	Enemy* enemy = new Enemy(player,enemyAttackCollider);
	enemyAttackCollider->SetEnemy(enemy);

	BackgroundObjectManager* bbObjectManager = new BackgroundObjectManager();

	TrackingCamera* trackingCamera   = new TrackingCamera(mainCamera_,player,enemy);

	/// 初期化する
	player->Initialize();
	playerHPRenderer->Initialize();
	trackingCamera->Initialize();
	enemy->Initialize();
	enemyAttackCollider->Initialize();

	bbObjectManager->Initialize();

	playerHPRenderer->SetPlayer(player);


	directionalLight_->SetDirection({0.0,-1.0f,0.0f});


	/// ui layer  index=1
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


}
