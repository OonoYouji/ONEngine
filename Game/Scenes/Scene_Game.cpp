#include "Scene_Game.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
#include <SceneManager.h>
#include <Input.h>

#include "GameMonitor/GameMonitor.h"
#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "Player/PlayerHP.h"
#include "GameOperationUI/GameOperationUI.h"
#include "LineDrawer2D/SinWaveDrawer.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyManager.h"
#include "GameTimer/GameTimer.h"
#include "Background/Background.h"
#include "GameManager/GameManager.h"
#include "EnemyComboManager.h"




/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {


	/// カメラの座標を計算
	BaseCamera* camera =
		CameraManager::GetInstance()->GetCamera("GameCamera");
	camera->SetMove(
		{ {0.0f, 0.2f, -15.0f}, { 0.0f, 0.0f, 0.0f } },
		{ {0.0f, 0.2f, -15.0f}, { 0.0f, -0.12f, 0.0f } },
		0.5f
	);


	/// 波
	SinWaveDrawer* sinWave_ = new SinWaveDrawer();
	sinWave_->Initialize();

	/// 敵
	Enemy* enemy_ = new Enemy();
	enemy_->SetWave(sinWave_);
	enemy_->Initialize();

	/// 敵pop
	EnemyManager* enemyManager_ = new EnemyManager();
	enemyManager_->Initialize();

	/// コンボの管理
	EnemyComboManager* enemyComboManager_ = new EnemyComboManager();
	enemyComboManager_->Initialize();

	/// プレイヤーのHP
	PlayerHP* playerHP = new PlayerHP();
	playerHP->Initialize();

	/// 心臓
	Heart* heart = new Heart;
	heart->Initialize();
	heart->SetPosition({ -3.8f, -0.8f, -4.1f });
	heart->SetRotate({ 0.0f, -1.0f, 0.45f });
	heart->SetScale(Vec3::kOne * 0.7f);
	heart->UpdateMatrix();

	/// 手
	Hand* hand = new Hand;
	hand->Initialize();
	hand->SetPosition({ -4.05f, -0.95f, -3.9f });
	hand->SetRotate({ 0.0f, -0.5f, 0.0f });
	hand->SetScale(Vec3::kOne * 0.5f);
	hand->UpdateMatrix();


	/// ゲームを映すモニター
	GameMonitor* gameMonitor = new GameMonitor;
	gameMonitor->Initialize();
	gameMonitor->isDrawActive = true;


	Background* gameBG = new Background;
	gameBG->Initialize();
	gameBG->drawLayerId = 1;


	Background* monitorBG = new Background;
	monitorBG->Initialize();
	monitorBG->SetColor({ 0, 0, 0, 1 });

	//(new GameCameraState)->Initialize();
	(new GameOperationUI)->Initialize();

	GameTimer* gameTimer = new GameTimer;
	gameTimer->Initialize();
	gameTimer->SetMaxTime(10.0f);

	GameManager* gameManager = new GameManager;
	gameManager->Initialize();
	enemyManager_->SetGameManager(gameManager);

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	/*if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager::GetInstance()->SetNextScene(TITLE);
	}*/
}


