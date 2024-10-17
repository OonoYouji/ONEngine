#include "Scene_Tutorial.h"

//std
#include<numbers>
#include "Scenes/Manager/SceneManager.h"
/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Ground/Ground.h"
#include "Objects/CameraState/GameCameraState.h"
#include "Objects/Tornado/Tornado.h"	
#include "Objects/Boss/Boss.h"	

void Scene_Tutorial::Initialize() {

	Ring::ResetInstanceCount();
	Wind::ResetInstanceCount();

	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	player_ = new Player;
	buildingManager_ = new BuildingManager();
	Ground* ground = new Ground;
	GameCameraState* gameCameraState = new GameCameraState();
	tornado_ = new Tornado();
	GameCamera* uiCamera = new GameCamera("UICamera");
	//チュートリアル
	tutorialScaleUpUI_ = new TutorialScaleUpUI();
	
	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	tornado_->Initialize();
	buildingManager_->Initialize();
	uiCamera->Initialize();
	//チュートリアル
	tutorialScaleUpUI_->Initialize();

	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player_);
	gameCameraState->SetDirectionalLight(directionalLight_);

	tornado_->SetPlayer(player_);
	AddLayer("ui", uiCamera);
	tutorialScaleUpUI_->drawLayerId=uiCamera->drawLayerId;
	//更新して、移動させない為にアクティブを切る
	player_->Update();
	buildingManager_->SetTornado(tornado_);

	player_->isActive = false;
	buildingManager_->isActive = false;
}
/// ===================================================
/// 更新処理
/// ===================================================
void Scene_Tutorial::Update() {
	buildingManager_->UpdateForTutorial();
	switch (tutorialState_) {
	case SCACLEUP: //竜巻のスケールアップ
		tutorialScaleUpUI_->isActive = true;
		// スケールダウンへ移行する条件
		if (tornado_->GetScaleScale() >= tornado_->GetMaxScale()) {
			tutorialScaleUpUI_->SetIsClose(true);
			if (tutorialScaleUpUI_->GetIsDeath()) {//UIが死んだら
				tutorialState_ = SCALEDOWN;  // 次の状態へ移行
			}
		}
		break;

	case SCALEDOWN:// スケールダウンモード	
		// 建物吸い込みへ移行する条件
		if (tornado_->GetScaleScale() <= tornado_->GetMinScale()) {
			tutorialState_ = BUILDINGSPOWN;  // 次の状態へ移行
		}
		break;

	case BUILDINGSPOWN:	// 建物生成の処理
		player_->isActive = true;
		buildingManager_->SpownBuilding(0, std::numbers::pi_v<float> / 0.4f);
		tutorialState_ = BUINDINGENTRAINMENT;  // 次の状態へ移行
		break;

	case BUINDINGENTRAINMENT://建物吸い込み
		//建物がなくなったら
		if (buildingManager_->GetSize() <= 0) {
			tutorialState_ = AFEWBUILDINGSPOWN;  // 次の状態へ移行
		}

		break;

	case AFEWBUILDINGSPOWN:	// 複数建物生成の処理
	
		buildingManager_->SpownBuilding(0, std::numbers::pi_v<float> / 1.5f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 1.2f, std::numbers::pi_v<float> / 1.2f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 1.4f, std::numbers::pi_v<float> / 1.2f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 1.2f, std::numbers::pi_v<float> / 1.8f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 1.3f, std::numbers::pi_v<float> / 1.3f);

		tutorialState_ = BUINDINGSENTRAINMENT;  // 次の状態へ移行
		break;

	case BUINDINGSENTRAINMENT:	// 複数建物生成の処理

		//建物がなくなったら
		if (buildingManager_->GetSize() <= 0) {
			dumy_ = new Dumy();
			dumy_->Initialize();
			tutorialState_ = DANYATTACK;  // 次の状態へ移行
		}
		break;


	case DANYATTACK:
		if (dumy_->GetIsBreak()) {
			SceneManager::GetInstance()->SetNextScene(SCENE_ID::BOSS_ENTRY);
		}
		break;

	default:
		// エラーハンドリングなどが必要ならここに書く
		break;
	}
}

////
//void Scene_Tutorial::CreateUI() {
//	if (!isCreateUI_) {
//		tutorialScaleUpUI_ = new TutorialScaleUpUI();
//		tutorialScaleUpUI_->Initialize();
//		isCreateUI_ = true;
//	}
//}