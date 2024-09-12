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
#include "EnemyComboManager/EnemyComboManager.h"
#include "Tile/Tile.h"
#include "Wall/Wall.h"
#include "ScoreManager/ScoreManager.h"



Scene_Game::~Scene_Game() {
	bgm_->StopAudioAll();
}

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize() {

	(new Tile)->Initialize();
	(new Wall)->Initialize();

	/// 波
	SinWaveDrawer* sinWave_ = new SinWaveDrawer();
	sinWave_->Initialize();

	/// 敵pop
	EnemyManager* enemyManager_ = new EnemyManager();
	enemyManager_->Initialize();

	/// コンボの管理
	EnemyComboManager* enemyComboManager_ = new EnemyComboManager();
	enemyComboManager_->Initialize();

	/// スコア
	ScoreManager* scoreManager = new ScoreManager();
	scoreManager->Initialize();

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

	(new GameOperationUI)->Initialize();

	GameTimer* gameTimer = new GameTimer;
	gameTimer->Initialize();
	gameTimer->SetMaxTime(60.0f);

	GameManager* gameManager = new GameManager;
	gameManager->Initialize();
	enemyManager_->SetGameManager(gameManager);



	bgm_ = new AudioSource;
	bgm_->SetAudioClip("BGM.wav");
	bgm_->isLoop = true;
	bgm_->PlayAudio();

}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update() {
	


}


