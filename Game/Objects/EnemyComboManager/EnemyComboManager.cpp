#include "EnemyComboManager.h"

#include <ImGuiManager.h>
#include "Enemy/Enemy.h"

EnemyComboManager::~EnemyComboManager()
{
}

void EnemyComboManager::Initialize()
{

	oneSprite_.reset(new Sprite());
	oneSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	twoSprite_.reset(new Sprite());
	twoSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	threeSprite_.reset(new Sprite());
	threeSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	fourSprite_.reset(new Sprite());
	fourSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	fiveSprite_.reset(new Sprite());
	fiveSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	sixSprite_.reset(new Sprite());
	sixSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	sevenSprite_.reset(new Sprite());
	sevenSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	eightSprite_.reset(new Sprite());
	eightSprite_->Initialize("SINON_enemy", "SINON_enemy.png");
	nineSprite_.reset(new Sprite());
	nineSprite_->Initialize("SINON_enemy", "SINON_enemy.png");

	comboSprite_.reset(new Sprite());
	comboSprite_->Initialize("SINON_enemy", "SINON_enemy.png");


}

void EnemyComboManager::Update()
{
	std::list<BaseGameObject*> objects = (GameObjectManager::GetGameObjectList("Enemy"));
	std::list<Enemy*> enemies;

	for (auto& gameObject : objects)
	{
		if (Enemy* enemy = dynamic_cast<Enemy*>(gameObject)) {
			enemies.push_back(enemy);
		}
	}

	for (auto& enemy : enemies)
	{
		if (enemy->IsCombo())
		{
			enemy->SetIsCombo(false);
			comboCount_++;
			comboReceptionTime_ = interruptTime_;

		}
	}

	if (comboReceptionTime_ > 0)
	{
		comboReceptionTime_--;
	}
	else if (comboReceptionTime_ == 0)
	{
		comboCount_ = 0;
		comboReceptionTime_ = 0;
	}


	objects.clear();
	enemies.clear();
}

void EnemyComboManager::FrontSpriteDraw()
{
}

void EnemyComboManager::Debug()
{
	if (ImGui::TreeNodeEx("Combo", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Combo:%d", comboCount_);
		ImGui::TreePop();
	}
}
