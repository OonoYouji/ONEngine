#include "Scene_Tutorial.h"

//std
#include<numbers>

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Ground/Ground.h"
#include "Objects/GameCameraState/GameCameraState.h"
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
	//Boss* boss = new Boss();
	tornado_ = new Tornado();

	/// ===================================================
	/// 初期化 : 順不同が最高だが、順番に関係があるなら要注意
	/// ===================================================

	player_->Initialize();
	ground->Initialize();
	gameCameraState->Initialize();
	tornado_->Initialize();
	/*boss->Initialize();*/
	buildingManager_->Initialize();

	/// ===================================================
	/// その他 セットするべきものをここに
	/// ===================================================

	gameCameraState->SetGameCamera(mainCamera_);
	gameCameraState->SetPlayer(player_);
	gameCameraState->SetDirectionalLight(directionalLight_);

	tornado_->SetPlayer(player_);

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

		// スケールダウンへ移行する条件
		if (tornado_->GetScale().x >= kTornadoScaleMax) {
			tutorialState_ = SCALEDOWN;  // 次の状態へ移行
		}
		break;

	case SCALEDOWN:// スケールダウンモード	
		// 建物吸い込みへ移行する条件
		if (tornado_->GetScale().x <= kTornadoScaleMin) {
			tutorialState_ = BUILDINGSPOWN;  // 次の状態へ移行
		}
		break;

	case BUILDINGSPOWN:	// 建物生成の処理
		player_->isActive = true;
		buildingManager_->SpownBuilding(0, std::numbers::pi_v<float> / 1.2f);
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
			tutorialState_ = DANYATTACK;  // 次の状態へ移行
		}
		break;


	case DANYATTACK:
		// 弾攻撃の処理を書くところ
		break;

	default:
		// エラーハンドリングなどが必要ならここに書く
		break;
	}
}
