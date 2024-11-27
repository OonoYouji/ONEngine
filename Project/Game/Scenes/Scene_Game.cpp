#include "Scene_Game.h"

/// std
#include <numbers>

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"
#include "FrameManager/Time.h"

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
#include "Objects/DefaultVignette/DefaultVignette.h"

#include "Objects/UIManager/UIManager.h"
#include "Objects/GameManagerObject/GameManagerObject.h"
#include "Objects/ModelPreviewObject/ModelPreviewObject.h"
#include "Objects/GameStartEffect/GameStartEffect.h"
#include "Objects/SceneTransition/SceneTransition.h"

/// ===================================================
/// 初期化処理
/// ===================================================
void Scene_Game::Initialize(){


	/// モデル確認用のオブジェクト
	(new ModelPreviewObject("Stage"))->Initialize();
	(new ModelPreviewObject("Effect12"))->Initialize();


	/// object creata
	gameManager_ = new GameManagerObject();


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
	DefaultVignette* defaultVignette = new DefaultVignette();

	TrackingCamera* trackingCamera   = new TrackingCamera(mainCamera_, player, enemy);


	/// objectの配列を宣言 start effectに渡す

	std::vector<BaseGameObject*> objectVec = {
		player, enemyAttackCollider, enemy, trackingCamera
	};

	GameStartEffect* gameStartEffect = new GameStartEffect(objectVec);


	/// 初期化する
	gameManager_->Initialize();

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
	defaultVignette->Initialize();

	gameStartEffect->Initialize();

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
	AddLayer("ui", uiCamera);


	GameCamera* transitionCamera = new GameCamera("transitionCamera");
	transitionCamera->Initialize();
	transitionCamera->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
	AddLayer("transitionLayer", transitionCamera);





	/// リスタート 処理をした
	const Flag& isGameRestart = GameManagerObject::GetFlag("isGameRestart");
	if(isGameRestart.Press()) {
		/// hpを半分からスタート
		enemy->SetHP(enemy->GetMaxHP() * 0.5f);
	}


	sceneTransition_ = nullptr;


	/// フラグのリセット
	GameManagerObject::SetFlag("isGameRestart", false);

	/// Loadがクソ重でdelta timeがバカでかくなり演出が吹き飛ぶので
	Time::GetInstance()->Update();
}


/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Game::Update(){

#ifdef _DEBUG
	if(Input::TriggerKey(KeyCode::Escape)){
		SceneManager::GetInstance()->SetNextScene(RESULT);
		gameManager_->SetFlag("isGameOver", true);
	}

	
	if(Input::TriggerKey(KeyCode::F1)){
		SceneManager::GetInstance()->SetNextScene(CLEAR);
		gameManager_->SetFlag("isGameClear", true);
	}
#endif // _DEBUG
	
	if(!isStartInTransition_) {
		if(!sceneTransition_) {
			isStartInTransition_ = true;
			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_OUT, 2.0f, GAME_SCENE_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		}
	}

	if(!isEndInTransition_) {
		if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
			sceneTransition_->Destory();
			sceneTransition_ = nullptr;
			isEndInTransition_ = true;
		}
	}


	if(!isStartOutTransition_) {

		if(gameManager_->GetFlag("isGameOver").Trigger()) {
			if(!sceneTransition_) {
				sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 2.0f, GAME_SCENE_LAYER_TRANSITION);
				sceneTransition_->Initialize();
				isStartOutTransition_ = true;
				nextScene_ = RESULT;
				return;
			}
		}

		if(gameManager_->GetFlag("isGameClear").Trigger()) {
			if(!sceneTransition_) {
				sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 2.0f, GAME_SCENE_LAYER_TRANSITION);
				sceneTransition_->Initialize();
				isStartOutTransition_ = true;
				nextScene_ = CLEAR;
				return;
			}
		}

	} else {

		if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
			SceneManager::GetInstance()->SetNextScene(static_cast<SCENE_ID>(nextScene_));
		}

	}


}
