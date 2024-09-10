#include "Scene_Game.h"

#include <ImGuiManager.h>
#include <ModelManager.h>
#include <CameraManager.h>
#include <SceneManager.h>
#include <Input.h>

#include "Player/Player.h"
#include "GameMonitor/GameMonitor.h"
#include "Heart/Heart.h"
#include "Hand/Hand.h"
#include "Shake/Shake.h"
#include "Player/PlayerHP.h"
#include "GameCameraState/GameCameraState.h"
#include "GameResult/GameResult.h"
#include "LineDrawer2D/SinWaveDrawer.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyManager.h"
#include "GamTimer/GameTimer.h"
#include "Background/Background.h"
#include "GameTitle/GameTitle.h"




/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

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
	(new GameResult)->Initialize();

	GameTimer* gameTimer = new GameTimer;
	gameTimer->Initialize();
	gameTimer->SetMaxTime(10.0f);


}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	/*if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager::GetInstance()->SetNextScene(TITLE);
	}*/
}


