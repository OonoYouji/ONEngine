#include "GameManagerObject.h"


/// objects
#include "Objects/Enemy/Enemy.h"
#include "Objects/Player/Player.h"


GameManagerObject::GameManagerObject() {
	CreateTag(this);
}

GameManagerObject::~GameManagerObject() {}

void GameManagerObject::Initialize() {

	flags_["isGameStart"];
	flags_["isGameClear"];
	flags_["isGameOver"];

}

void GameManagerObject::Update() {

	for(auto& flag : flags_) {
		flag.second.Update();
	}


	/// 敵を倒した
	if(pEnemy_->GetHP() <= 0.0f) {
		flags_.at("isGameClear").current = true;
	}
	
	/// プレイヤーが死んだ
	if(pPlayer_->GetCurrentHP() <= 0.0f) {
		flags_.at("isGameOver").current = true;
	}


}

void GameManagerObject::SetEnemy(Enemy* _enemy) {
	pEnemy_ = _enemy;
}

void GameManagerObject::SetPlayer(Player* _player) {
	pPlayer_ = _player;
}

const Flag& GameManagerObject::GetFlag(const std::string& _key) {
	return flags_.at(_key);
}

