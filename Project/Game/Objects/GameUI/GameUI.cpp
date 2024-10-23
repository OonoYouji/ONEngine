#include "GameUI.h"

#include "ComponentManager/MeshRenderer/MeshRenderer.h"

/// objects
#include "Objects/PowerUpGauge/PowerUpGauge.h"
#include "BossHP/BossHP.h"
#include "Objects/Boss/Boss.h"
#include "Objects/Player/Player.h"


GameUI::GameUI() {
	CreateTag(this);
}

GameUI::~GameUI() {}

void GameUI::Initialize() {

	powerUpGauge_ = new PowerUpGauge();
	powerUpGauge_->Initialize();
	powerUpGauge_->SetParent(pTransform_);

	bossHP_ = new BossHP();
	bossHP_->Initialize();
	bossHP_->SetParent(pTransform_);

	drawLayerId = 1;
	powerUpGauge_->drawLayerId = 1;
	bossHP_->drawLayerId = 1;

	pTransform_->position = { 0.0f, 0.0f, 4.4f };

}

void GameUI::Update() {

}

void GameUI::SetPlayer(Player* _player) {
	powerUpGauge_->SetPlayer(_player);
}

void GameUI::SetBoss(Boss* _boss) {
	bossHP_->SetBoss(_boss);
}
