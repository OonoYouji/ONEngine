#include "EnemyManager.h"

#include <GameObjectManager.h>
#include <ImGuiManager.h>
#include <Input.h>
#include <fstream>
#include "Enemy/Enemy.h"
#include "Drawer/LineDrawer2D/SinWaveDrawer.h"
#include "GameManager/GameManager.h"


void EnemyManager::Initialize() {

	LoadPopDate("EnemyPop.csv");
}

void EnemyManager::Update() {

	PopCommands();
}

void EnemyManager::EnemyPop()
{
	Enemy* newEnemy = new Enemy();
	newEnemy->SetWave(dynamic_cast<SinWaveDrawer*>(GameObjectManager::GetGameObject("SinWaveDrawer")));
	newEnemy->Initialize();
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
	if (popCount_ == 0 && popInterval_ == 0 && commands_.size() != 0) {
		popCount_ = commands_.front().first;
		popInterval_ = commands_.front().second;
		commands_.pop_front();
		currentInterval_ = popInterval_;
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
		if (waitTimer_ <= 0) {
			isWait_ = false;
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
			int32_t waitTime = (int32_t)(atoi(word.c_str()) * 60.0f);

			isWait_ = true;
			waitTimer_ = waitTime;

			break;
		}

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
