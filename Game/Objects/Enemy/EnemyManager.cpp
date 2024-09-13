#include "EnemyManager.h"

#include <GameObjectManager.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <fstream>
#include "Enemy/Enemy.h"
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"
#include "GameManager/GameManager.h"


EnemyManager::~EnemyManager()
{
	commands_.clear();
}

void EnemyManager::Initialize() {

	LoadPopDate("EnemyPop.csv");

	warningSprite_.reset(new Sprite());
	warningSprite_->Initialize("warning", "warning.png");
	warningSprite_->SetSize({ 40.0f,40.0f });
	warningSprite_->SetPos({ 1220,200,0 });

	alarm_ = new AudioSource();
	alarm_->SetAudioClip("warning.wav");

}

void EnemyManager::Update() {

	uint32_t enemyCount = GameObjectManager::GetInstanceCount("Enemy");

	PopCommands();
	WarningUpdate();
}

void EnemyManager::FrontSpriteDraw()
{

	warningSprite_->Draw();

}

void EnemyManager::EnemyPop()
{
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize();
	GameManager* gameManager = dynamic_cast<GameManager*>(GameObjectManager::GetGameObject("GameManager"));
	gameManager->AddPausedObject(newEnemy);
}

void EnemyManager::LoadPopDate(const std::string& fileName)
{
	std::ifstream file;
	file.open(kDirectoryPath_ + fileName);
	assert(file.is_open());

	popCommand_ << file.rdbuf();

	file.close();
}

void EnemyManager::PopCommands()
{
	uint32_t enemyCount = GameObjectManager::GetInstanceCount("Enemy");


	if (popCount_ == 0 && popInterval_ == 0 && commands_.size() != 0) {
		popCount_ = commands_.front().first;
		popInterval_ = commands_.front().second;
		commands_.pop_front();
		if (enemyCount+popCount_ >= maxPopCount_) {
			isStop_ = true;
		}
	}


	if (popCount_ > 0) {
		if (currentInterval_ == 0)
		{
			EnemyPop();
			popCount_--;
			currentInterval_ = popInterval_;
		}
	} else if (popCount_ == 0 && currentInterval_ == 0) {
		popInterval_ = 0;
	}

	if (currentInterval_ > 0) {
		currentInterval_--;
	}


	if (isWait_) {
		waitTimer_--;
		if (waitTimer_ == 120)
		{
			isWarning_ = true;
			warningPosition_ = { 1320.0f,200.0f,0.0f };
			warningSize_ = { 40.0f,40.0f,0.0f };
			warningSprite_->SetSize({ warningSize_.x,warningSize_.y });
			alarm_->PlayAudio();
		}
		if (waitTimer_ <= 0) {
			isWait_ = false;
			isWarning_ = false;
		}
		return;
	}

	if (isStop_) {
		if (enemyCount <= 4) {
			isStop_ = false;
		}
		return;
	}

	std::string line;
	while (std::getline(popCommand_, line)) {

		std::istringstream line_stream(line);
		std::string word;

		//,区切りで行の先頭文字列取得
		std::getline(line_stream, word, ',');

		if (word.find("//") == 0) {
			continue;//コメント行飛ばす
		}
		//POP
		if (word.find("POP") == 0)
		{

			// 出す数
			std::getline(line_stream, word, ',');
			int count = (int)std::atof(word.c_str());
			// インターバル
			std::getline(line_stream, word, ',');
			int interval = (int)(std::atof(word.c_str()) * 60.0f);

			commands_.push_back(std::make_pair(count, interval));

		}
		//WAIT
		else if (word.find("WAIT") == 0) {
			std::getline(line_stream, word, ',');

			//待ち時間
			uint32_t waitTime = (uint32_t)(atoi(word.c_str()) * 60.0f);

			isWait_ = true;
			waitTimer_ = waitTime;

			break;
		}

	}

}

void EnemyManager::WarningUpdate() {
	if (isWarning_)
	{
		warningPosition_ = Vector3::Lerp(warningPosition_, { 1190.0f,200.0f,0.0f }, 0.15f);
		warningSprite_->SetPos(warningPosition_);
	}
	else
	{
		warningSize_ = Vector3::Lerp(warningSize_, { 0,0,0 }, 0.2f);
		warningSprite_->SetSize({ warningSize_.x,warningSize_.y });
	}
}

void EnemyManager::Debug() {
	if(ImGui::TreeNodeEx("EnemyManager", ImGuiTreeNodeFlags_DefaultOpen)) {
		if(ImGui::Button("Enemy Pop")) {
			EnemyPop();
		}

		ImGui::TreePop();
	}
}

void EnemyManager::SetGameManager(GameManager* gameManager) {
	pGameManager_ = gameManager;
}
