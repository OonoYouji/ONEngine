#include"PlayerPowerUp.h"
#include"Objects/PlayerBehavior/PlayerRoot.h"
#include"Objects/Player/Player.h"

//初期化
PlayerPowerUp::PlayerPowerUp(Player* player)
	: BasePlayerBehavior("Behavior PowerUp", player) {

	pPlayer_->PowerUpInit();
}

PlayerPowerUp::~PlayerPowerUp() {

}

//更新
void PlayerPowerUp::Update() {
	pPlayer_->PowerUpUpdate();
	if (pPlayer_->GetPowerUpTime() <= 0) {
		pPlayer_->ChangeState(std::make_unique<PlayerRoot>(pPlayer_));
	}
}

