#include "EnemyComboManager.h"

#include <ImGuiManager.h>
#include "Enemy/Enemy.h"

uint32_t EnemyComboManager::killedEnemiesCount_ = 0u;

EnemyComboManager::~EnemyComboManager()
{
}

void EnemyComboManager::Initialize()
{

	killedEnemiesCount_ = 0u;

	oneSprite_.reset(new Sprite());
	oneSprite_->Initialize("1", "1.png");
	oneSprite_->SetSize({ 20,20 });
	twoSprite_.reset(new Sprite());
	twoSprite_->Initialize("2", "2.png");
	twoSprite_->SetSize({ 20,20 });
	threeSprite_.reset(new Sprite());
	threeSprite_->Initialize("3", "3.png");
	threeSprite_->SetSize({ 20,20 });
	fourSprite_.reset(new Sprite());
	fourSprite_->Initialize("4", "4.png");
	fourSprite_->SetSize({ 20,20 });
	fiveSprite_.reset(new Sprite());
	fiveSprite_->Initialize("5", "5.png");
	fiveSprite_->SetSize({ 20,20 });
	sixSprite_.reset(new Sprite());
	sixSprite_->Initialize("6", "6.png");
	sixSprite_->SetSize({ 20,20 });
	sevenSprite_.reset(new Sprite());
	sevenSprite_->Initialize("7", "7.png");
	sevenSprite_->SetSize({ 20,20 });
	eightSprite_.reset(new Sprite());
	eightSprite_->Initialize("8", "8.png");
	eightSprite_->SetSize({ 20,20 });
	nineSprite_.reset(new Sprite());
	nineSprite_->Initialize("9", "9.png");
	nineSprite_->SetSize({ 20,20 });


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
			/// 倒した敵の増加
			killedEnemiesCount_++;

			enemy->SetIsCombo(false);
			comboCount_++;
			comboReceptionTime_ = interruptTime_;
			spritePos_ = enemy->GetPos();
			spritePos_.y -= 60.0f;
			switch (comboCount_)
			{
			case 1:
				oneSprite_->SetPos(spritePos_);
				break;
			case 2:
				twoSprite_->SetPos(spritePos_);
				break;
			case 3:
				threeSprite_->SetPos(spritePos_);
				break;
			case 4:
				fourSprite_->SetPos(spritePos_);
				break;
			case 5:
				fiveSprite_->SetPos(spritePos_);
				break;
			case 6:
				sixSprite_->SetPos(spritePos_);
				break;
			case 7:
				sevenSprite_->SetPos(spritePos_);
				break;
			case 8:
				eightSprite_->SetPos(spritePos_);
				break;
			case 9:
				nineSprite_->SetPos(spritePos_);
				break;
			}
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
	switch (comboCount_)
	{
	case 0:
		break;
	case 1:
		oneSprite_->Draw();
		break;
	case 2:
		twoSprite_->Draw();
		break;
	case 3:
		threeSprite_->Draw();
		break;
	case 4:
		fourSprite_->Draw();
		break;
	case 5:
		fiveSprite_->Draw();
		break;
	case 6:
		sixSprite_->Draw();
		break;
	case 7:
		sevenSprite_->Draw();
		break;
	case 8:
		eightSprite_->Draw();
		break;
	case 9:
		nineSprite_->Draw();
		break;
	}
}

void EnemyComboManager::Debug()
{
	if (ImGui::TreeNodeEx("Combo", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Combo:%d", comboCount_);
		ImGui::TreePop();
	}
}
