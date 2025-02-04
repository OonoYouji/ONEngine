#include "EnemyHPRenderer.h"

/// std
#include <algorithm>

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "Library/Debugger/Assertion.h"

/// user
#include "Scenes/Scene_Game.h"
#include "../Enemy.h"
#include "EnemyHPBarGauge/EnemyHPBarGauge.h"
#include "EnemyHPBarFrame/EnemyHPBarFrame.h"
#include "Objects/Camera/Manager/CameraManager.h"


EnemyHPRenderer::EnemyHPRenderer(Enemy* _enemyPtr)
	: pEnemy_(_enemyPtr) {
	CreateTag(this);
}

EnemyHPRenderer::~EnemyHPRenderer() {}

void EnemyHPRenderer::Initialize() {

	/// gaugeの初期化
	hpBarGauge_ = new EnemyHPBarGauge();
	hpBarGauge_->Initialize();
	hpBarGauge_->SetParent(pTransform_);

	/// frameの初期化
	hpBarFrame_ = new EnemyHPBarFrame();
	hpBarFrame_->Initialize();
	hpBarFrame_->SetParent(pTransform_);

	
	pTransform_->position.x = -1.7f;
	pTransform_->position.y = 1.0f;

	drawLayerId = GAME_SCENE_LAYER_UI;

}

void EnemyHPRenderer::Update() {

	CameraManager* cameraManager = CameraManager::GetInstance();
	pMainCamera_ = cameraManager->GetMainCamera();

	if(pMainCamera_) {
		/// ビルボード計算
		pTransform_->rotate = pMainCamera_->GetRotate();
		pTransform_->scale  = pMainCamera_->GetScale();
	}

	/// HPの割合から表示するゲージの長さを計算
	float hpRate = pEnemy_->GetHP() / pEnemy_->GetMaxHP();
	hpRate = std::clamp(hpRate, 0.0f, 1.0f); /// 0~1の間に収める
	hpBarGauge_->SetScale(Vec3(hpRate, 1.0f, 1.0f));

	pTransform_->position = pEnemy_->GetPosition() + Vec3(0.0f, 2.0f, 0.0f);

}

