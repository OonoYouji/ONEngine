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
#include <GraphicManager/TextureManager/TextureManager.h>

void Scene_Tutorial::Initialize() {

	Wind::ResetInstanceCount();

	/// ===================================================
	/// ゲームオブジェクトの宣言
	/// ===================================================

	player_ = new Player;
	buildingManager_ = new BuildingManager();
	Ground* ground = new Ground;
	GameCameraState* gameCameraState = new GameCameraState();
	tornado_ = new Tornado();
	GameCamera* uiCamera = new GameCamera("TutorialUICamera");
	guidanceArrow_ = new GuidanceArrow();
	//チュートリアル
	tutorialScaleUpUI_ = new TutorialScaleUpUI();
	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================
	/*TextureManager::Load("Tutorial1.png"); textureManager->Load("uvChecker", "uvChecker.png");*/
	//チュートリアル
	tutorialScaleUpUI_->Initialize();
	
	player_->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	tornado_->Initialize();
	buildingManager_->Initialize();
	uiCamera->Initialize();
	guidanceArrow_->Initialize();
	
	/*TextureManager::GetInstance()->Load("tutorial1", "tutorial1.png");
	TextureManager::GetInstance()->Load("tutorial2", "tutorial2.png");
	TextureManager::GetInstance()->Load("tutorial3", "tutorial3.png");
	TextureManager::GetInstance()->Load("tutorial4", "tutorial4.png");*/

	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player_);
	gameCameraState->SetDirectionalLight(directionalLight_);

	player_->SetTornado(tornado_);
	tornado_->SetPlayer(player_);
	guidanceArrow_->SetPlayer(player_);
	AddLayer("ui", uiCamera);
	uiCamera->SetPositionZ(-1.4f);
	//更新して、移動させない為にアクティブを切る
	player_->Update();
	buildingManager_->SetTornado(tornado_);
	player_->isActive = false;
	buildingManager_->isActive = false;
	//アクティブ切るUI

	guidanceArrow_->isActive = false;

	tutorialState_ = SCACLEUP;
	
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
		if (tornado_->GetScaleScaler() >= tornado_->GetMaxScale()) {
			tornado_->SetIsInputreception(true);///　入力のみを受け付けなくする
			tutorialScaleUpUI_->SetIsClose(true);
			if (tutorialScaleUpUI_->GetIsDeath()) {//UIが死んだら
				tornado_->SetIsInputreception(false);
				tutorialScaleDownUI_ = new TutorialScaleDownUI();
				tutorialScaleDownUI_->Initialize();
				tutorialState_ = SCALEDOWN;  // 次の状態へ移行
			}
		}
		break;

	case SCALEDOWN:// スケールダウンモード	
   		tutorialScaleDownUI_->isActive = true;
		tornado_->isActive = true;
		// 建物吸い込みへ移行する条件
		if (tornado_->GetScaleScaler() <= tornado_->GetMinScale()) {
			tutorialScaleDownUI_->SetIsClose(true);
			if (tutorialScaleDownUI_->GetIsDeath()) {//UIが死んだら
				tutorialEntrainment_ = new TutorialEntrainment();
				tutorialEntrainment_->Initialize();
				tutorialState_ = BUILDINGSPOWN;  // 次の状態へ移行
			}
		}
		break;

	case BUILDINGSPOWN:	// 建物生成の処理
		player_->isActive = true;
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 6.4f,0);
		tutorialState_ = BUINDINGENTRAINMENT;  // 次の状態へ移行
		break;

	case BUINDINGENTRAINMENT://建物吸い込み
		
		tutorialEntrainment_->isActive = true;
		//建物がなくなったら
		if (buildingManager_->GetSize() <= 0) {
			tutorialEntrainment_->SetIsClose(true);
			if (tutorialEntrainment_->GetIsDeath()) {//UIが死んだら
				tutorialEntrainmentAll_ = new TutorialEntrainmentAll();
				tutorialEntrainmentAll_->Initialize();
				tutorialState_ = AFEWBUILDINGSPOWN;  // 次の状態へ移行
			}		
		}

		break;

	case AFEWBUILDINGSPOWN:	// 複数建物生成の処理

		buildingManager_->SpownBuilding(0, std::numbers::pi_v<float> / 5.0f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 5.0f, std::numbers::pi_v<float> / 5.0f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 3.3f, std::numbers::pi_v<float> / 4.7f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 7.3f, std::numbers::pi_v<float> / 7.0f);
		buildingManager_->SpownBuilding(std::numbers::pi_v<float> / 2.4f, std::numbers::pi_v<float> / 10.4f);

		tutorialState_ = BUINDINGSENTRAINMENT;  // 次の状態へ移行
		break;

	case BUINDINGSENTRAINMENT:	// 複数建物巻きこめ
		
		tutorialEntrainmentAll_->isActive = true;
		//建物がなくなったら
		if (buildingManager_->GetSize() <= 0) {
			tutorialEntrainmentAll_->SetIsClose(true);
			if (tutorialEntrainmentAll_->GetIsDeath()) {//UIが死んだら
				tutorialState_ = DANYATTACK;  // 次の状態へ移行
				tutorialBodyBlow_ = new TutorialBodyBlow();
				tutorialBodyBlow_->Initialize();
				dumy_ = new Dumy();
				dumy_->Initialize();
				dumy_->SetPlyer(player_);
			}
		}
		break;


	case DANYATTACK:
		tutorialBodyBlow_->isActive = true;
		guidanceArrow_->UpdateForTutorial(dumy_->GetPos());
		if (dumy_->GetIsBreak()) {
			tutorialBodyBlow_->SetIsClose(true);
			if (tutorialBodyBlow_->GetIsDeath()) {//UIが死んだら
				SceneManager::GetInstance()->SetNextScene(SCENE_ID::BOSS_ENTRY);
			}	
		}
		break;

	default:
		// エラーハンドリングなどが必要ならここに書く
		break;
	}
}
