#include "EnemyComboManager.h"

#include <ImGuiManager.h>
#include "Enemy/Enemy.h"

EnemyComboManager::~EnemyComboManager()
{
}

void EnemyComboManager::Initialize()
{
	float sizex = 40;

	oneSprite_.reset(new Sprite());
	oneSprite_->Initialize("x1", "x1.png");
	oneSprite_->SetSize({ sizex,20 });

	twoSprite_.reset(new Sprite());
	twoSprite_->Initialize("x2", "x2.png");
	twoSprite_->SetSize({ sizex,20 });

	threeSprite_.reset(new Sprite());
	threeSprite_->Initialize("x3", "x3.png");
	threeSprite_->SetSize({ sizex,20 });

	fourSprite_.reset(new Sprite());
	fourSprite_->Initialize("x4", "x4.png");
	fourSprite_->SetSize({ sizex,20 });

	fiveSprite_.reset(new Sprite());
	fiveSprite_->Initialize("x5", "x5.png");
	fiveSprite_->SetSize({ sizex,20 });

	sixSprite_.reset(new Sprite());
	sixSprite_->Initialize("x6", "x6.png");
	sixSprite_->SetSize({ sizex,20 });

	sevenSprite_.reset(new Sprite());
	sevenSprite_->Initialize("x7", "x7.png");
	sevenSprite_->SetSize({ sizex,20 });

	eightSprite_.reset(new Sprite());
	eightSprite_->Initialize("x8", "x8.png");
	eightSprite_->SetSize({ sizex,20 });

	nineSprite_.reset(new Sprite());
	nineSprite_->Initialize("x9", "x9.png");
	nineSprite_->SetSize({ sizex,20 });


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
			spritePos_ = enemy->GetPos();
			spritePos_.y -= 60.0f;
			spriteSize_ = { 120,60,0 };
			switch (comboCount_)
			{
			case 1:
				oneSprite_->SetPos(spritePos_);
				oneSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 2:
				twoSprite_->SetPos(spritePos_);
				twoSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 3:
				threeSprite_->SetPos(spritePos_);
				threeSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 4:
				fourSprite_->SetPos(spritePos_);
				fourSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 5:
				fiveSprite_->SetPos(spritePos_);
				fiveSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 6:
				sixSprite_->SetPos(spritePos_);
				sixSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 7:
				sevenSprite_->SetPos(spritePos_);
				sevenSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 8:
				eightSprite_->SetPos(spritePos_);
				eightSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			case 9:
				nineSprite_->SetPos(spritePos_);
				nineSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
				break;
			}
		}
	}

	switch (comboCount_) {
	case 1:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		oneSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 2:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		twoSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 3:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		threeSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 4:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		fourSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 5:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		fiveSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 6:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		sixSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 7:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		sevenSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 8:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		eightSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
	case 9:
		spriteSize_ = Vector3::Lerp(spriteSize_, { 40,20,0 }, 0.2f);
		nineSprite_->SetSize({ spriteSize_.x,spriteSize_.y });
		break;
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
