#include "GameManagerObject.h"

#include "FrameManager/Time.h"

/// objects
#include "Objects/Enemy/Enemy.h"
#include "Objects/Player/Player.h"

std::unordered_map<std::string, Flag> GameManagerObject::flags_ = {
	{ "isGameStart", Flag()},
	{ "isGameClear", Flag()},
	{ "isGameOver", Flag()},
	{ "isGameRestart", Flag()}
};
float GameManagerObject::gameClearedTime_ = 0.0f;


GameManagerObject::GameManagerObject() {
	CreateTag(this);
}

GameManagerObject::~GameManagerObject() {}

void GameManagerObject::Initialize() {

	if(!flags_["isGameRestart"].Press()) {
		gameClearedTime_ = 0.0f;
	}

	flags_["isGameStart"] = {};
	flags_["isGameClear"] = {};
	flags_["isGameOver"] = {};


}

void GameManagerObject::Update() {

	if(!flags_["isGameOver"].Press() || !flags_["isGameClear"].Press()) {
		gameClearedTime_ += Time::DeltaTime();
	}

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

void GameManagerObject::SetFlag(const std::string& _key, bool value) {
	flags_[_key].current = value;
}

float GameManagerObject::GetClearTime() {
	return gameClearedTime_;
}

