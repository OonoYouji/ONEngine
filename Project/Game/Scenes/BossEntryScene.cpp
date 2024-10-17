#include "BossEntryScene.h"

#include "Objects/Camera/GameCamera.h"
#include "Objects/BossAnimation/BossAnimation.h"

void BossEntryScene::Initialize() {


	/// main camera setting
	mainCamera_->GetTransform()->rotateOrder = XYZ;


	/// objects instance create
	BossAnimation* bossAnimation = new BossAnimation();

	bossAnimation->Initialize();
	bossAnimation->SetGameCamera(mainCamera_);
	bossAnimation->SetAnimationIndex(BOSS_ANIMATION_ENTRY_CAMERA_MOVE);

}

void BossEntryScene::Update() {

}
