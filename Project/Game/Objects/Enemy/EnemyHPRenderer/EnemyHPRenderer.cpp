#include "EnemyHPRenderer.h"

/// engine
#include "ComponentManager/MeshRenderer/MeshRenderer.h"
#include "ComponentManager/SpriteRenderer/SpriteRenderer.h"

#include "Library/Debugger/Assertion.h"

/// user
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

}

void EnemyHPRenderer::Update() {

	CameraManager* cameraManager = CameraManager::GetInstance();
	pMainCamera_ = cameraManager->GetMainCamera();

	if(pMainCamera_) {

		/// ビルボード計算
		pTransform_->rotate = pMainCamera_->GetTransform()->rotate;
		pTransform_->scale  = pMainCamera_->GetTransform()->scale;
	}

}

