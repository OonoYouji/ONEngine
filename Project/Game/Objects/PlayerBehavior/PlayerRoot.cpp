#include"PlayerRoot.h"
#include"Objects/PlayerBehavior/PlayerPowerUp.h"
#include"Objects/Player/Player.h"

//初期化
PlayerRoot::PlayerRoot(Player* player)
	: BasePlayerBehavior("Behavior Root", player) {

	pPlayer_->RootInit();
}

PlayerRoot::~PlayerRoot() {

}

//更新
void PlayerRoot::Update() {
	
}

