#include "GameManagerObject.h"


/// objects
#include "Objects/Enemy/Enemy.h"
#include "Objects/Player/Player.h"

std::unordered_map<std::string, Flag> GameManagerObject::flags_;
std::chrono::high_resolution_clock::time_point GameManagerObject::startTime_;
std::chrono::high_resolution_clock::time_point GameManagerObject::endTime_;


GameManagerObject::GameManagerObject() {
	CreateTag(this);
}

GameManagerObject::~GameManagerObject() {}

void GameManagerObject::Initialize() {

	startTime_ = std::chrono::high_resolution_clock::now();

	flags_["isGameStart"] = {};
	flags_["isGameClear"] = {};
	flags_["isGameOver"] = {};
	flags_["isGameRestart"] = {};

}

void GameManagerObject::Update() {

	for(auto& flag : flags_) {
		flag.second.Update();
	}


	/// 敵を倒した
	if(pEnemy_->GetHP() <= 0.0f) {
		flags_.at("isGameClear").current = true;
		endTime_ = std::chrono::high_resolution_clock::now();
	}
	
	/// プレイヤーが死んだ
	if(pPlayer_->GetCurrentHP() <= 0.0f) {
		flags_.at("isGameOver").current = true;
		endTime_ = std::chrono::high_resolution_clock::now();
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
	endTime_ = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float, std::milli> duration = endTime_ - startTime_;
	float time = duration.count() / 1000.0f;
	return time;
}

