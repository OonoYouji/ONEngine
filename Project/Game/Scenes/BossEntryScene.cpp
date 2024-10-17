#include "BossEntryScene.h"

/// engine
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "GraphicManager/Light/DirectionalLight.h"

#include "Objects/BossAnimation/BossAnimation.h"
#include "Objects/TitleObjects/TitleEarth.h"
#include "Objects/CameraState/CameraStateBossEntryToGame.h"


void BossEntryScene::Initialize() {


	/// main camera setting
	mainCamera_->GetTransform()->rotateOrder = XYZ;

	directionalLight_->SetDirection(Vec3(1.0f, 0.f, 1.0f).Normalize());

	/// objects instance create
	bossAnimation_ = new BossAnimation();
	cameraState_   = new CameraStateBossEntryToGame();
	TitleEarth* earth = new TitleEarth();

	bossAnimation_->Initialize();
	cameraState_->Initialize();
	earth->Initialize();

	bossAnimation_->SetPosition({ 0.0f, 11.0f, 0.0f });
	bossAnimation_->SetGameCamera(mainCamera_);
	bossAnimation_->SetAnimationIndex(BOSS_ANIMATION_ENTRY_CAMERA_MOVE);

	cameraState_->SetGameCamera(mainCamera_);
	cameraState_->isActive = false;

}

void BossEntryScene::Update() {

	const AnimationData& data = bossAnimation_->GetAnimatinData(BOSS_ANIMATION_ENTRY_TUBE_DOWN);

	/// 振り下ろしが終了したので遷移する
	if(data.time / data.maxTime >= 1.0f) {
		//SceneManager::GetInstance()->SetNextScene(GAME);
		cameraState_->isActive = true;
	}

}
