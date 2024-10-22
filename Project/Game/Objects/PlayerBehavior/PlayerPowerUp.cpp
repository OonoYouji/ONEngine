#include"PlayerPowerUp.h"
#include"Objects/PlayerBehavior/PlayerRoot.h"
#include"Objects/Player/Player.h"

#include "Objects/CameraState/GameCameraState.h"

//初期化
PlayerPowerUp::PlayerPowerUp(Player* player)
	: BasePlayerBehavior("Behavior PowerUp", player) {

	pPlayer_->PowerUpInit();
}

PlayerPowerUp::~PlayerPowerUp() {

}

//更新
void PlayerPowerUp::Update() {

	/// time rateの更新
	pPlayer_->TimeRateUpdate();
	if(pPlayer_->GetTimeRate() >= 1.0f) {

		/// カメラをズームアウトする(一回だけ)
		if(!isCameraZoomOut_) {
			isCameraZoomOut_ = true;

			pPlayer_->SetCameraBehavior(static_cast<int>(CameraBehavior::kZoomOut));
		}

	}

	/// behaviorの更新
	pPlayer_->PowerUpUpdate();

	/// 終了したら元の振る舞いに戻る
	if (pPlayer_->GetPowerUpTime() <= 0) {
		/// カメラの振る舞いをrootに戻す
		pPlayer_->SetCameraBehavior(static_cast<int>(CameraBehavior::kRoot));

		pPlayer_->ChangeState(std::make_unique<PlayerRoot>(pPlayer_));
	}
}

