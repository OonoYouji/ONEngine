#include "ScoreManager.h"

#include "ImGuiManager.h"
#include <Easing.h>
#include <WorldTime.h>

#include "Enemy/Enemy.h"
#include "LineDrawer2D/SinWaveDrawer.h"


uint32_t ScoreManager::scoreCount_ = 0u;


ScoreManager::~ScoreManager() {}

void ScoreManager::Initialize() {

	for(int i = 0; i < 10; i++) {
		std::unique_ptr<Sprite> digitOne;
		digitOne.reset(new Sprite());
		digitOne->Initialize(std::to_string(i), std::to_string(i) + ".png");
		digitOne->SetSize({ 30,36 });
		digitOne->SetPos({ 1180,100,0 });
		digitOneSprites_.push_back(std::move(digitOne));

		float addPosx = -60.0f;
		float count = 1;

		std::unique_ptr<Sprite> digitTwo;
		digitTwo.reset(new Sprite());
		digitTwo->Initialize(std::to_string(i), std::to_string(i) + ".png");
		digitTwo->SetSize({ 30,36 });
		digitTwo->SetPos({ 1180 + (addPosx * count),100,0 });
		digitTwoSprites_.push_back(std::move(digitTwo));

		count++;

		std::unique_ptr<Sprite> digitThree;
		digitThree.reset(new Sprite());
		digitThree->Initialize(std::to_string(i), std::to_string(i) + ".png");
		digitThree->SetSize({ 30,36 });
		digitThree->SetPos({ 1180 + (addPosx * count),100,0 });
		digitThreeSprites_.push_back(std::move(digitThree));

		count++;

		std::unique_ptr<Sprite> digitFour;
		digitFour.reset(new Sprite());
		digitFour->Initialize(std::to_string(i), std::to_string(i) + ".png");
		digitFour->SetSize({ 30,36 });
		digitFour->SetPos({ 1180 + (addPosx * count),100,0 });
		digitFourSprites_.push_back(std::move(digitFour));

		count++;

		std::unique_ptr<Sprite> digitFive;
		digitFive.reset(new Sprite());
		digitFive->Initialize(std::to_string(i), std::to_string(i) + ".png");
		digitFive->SetSize({ 30,36 });
		digitFive->SetPos({ 1180 + (addPosx * count),100,0 });
		digitFiveSprites_.push_back(std::move(digitFive));

		count++;

		std::unique_ptr<Sprite> digitSix;
		digitSix.reset(new Sprite());
		digitSix->Initialize(std::to_string(i), std::to_string(i) + ".png");
		digitSix->SetSize({ 30,36 });
		digitSix->SetPos({ 1180 + (addPosx * count),100,0 });
		digitSixSprites_.push_back(std::move(digitSix));
	}

	scoreCount_ = 0;

}

void ScoreManager::Update() {

	std::list<BaseGameObject*> objects = (GameObjectManager::GetGameObjectList("Enemy"));
	std::list<Enemy*> enemies;

	for(auto& gameObject : objects) {
		if(Enemy* enemy = dynamic_cast<Enemy*>(gameObject)) {
			enemies.push_back(enemy);
		}
	}

	for(auto& enemy : enemies) {
		if(enemy->IsScore()) {
			enemy->SetIsScore(false);
			highPoint_ = enemy->GetHighPoint();



			scoreCount_ += baseScore_;
		}
	}

	CalScoreSprite();

	objects.clear();
	enemies.clear();
}

void ScoreManager::FrontSpriteDraw() {

	/*digitOneSprites_[oneCount_]->Draw();
	digitTwoSprites_[twoCount_]->Draw();
	digitThreeSprites_[threeCount_]->Draw();
	digitFourSprites_[fourCount_]->Draw();
	digitFiveSprites_[fiveCount_]->Draw();
	digitSixSprites_[sixCount_]->Draw();*/

}

void ScoreManager::Debug() {
	if(ImGui::TreeNodeEx("Score", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text("%d", scoreCount_);
		ImGui::TreePop();
	}
}

void ScoreManager::CalScoreSprite() {

	if(scoreCount_ >= 1000000) {
		oneCount_ = 9;
		twoCount_ = 9;
		threeCount_ = 9;
		fourCount_ = 9;
		fiveCount_ = 9;
		sixCount_ = 9;
		return;
	}

	uint32_t point = scoreCount_;
	oneCount_ = point % 10;
	point = point / 10;

	twoCount_ = point % 10;
	point = point / 10;

	threeCount_ = point % 10;
	point = point / 10;

	fourCount_ = point % 10;
	point = point / 10;

	fiveCount_ = point % 10;
	point = point / 10;

	sixCount_ = point;

}

void ScoreManager::CalScore() {

	float highPoint = highPoint_;


}
