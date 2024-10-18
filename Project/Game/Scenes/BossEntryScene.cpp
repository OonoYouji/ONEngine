#include "BossEntryScene.h"

/// std
#include <numbers>

/// engine
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "GraphicManager/Light/DirectionalLight.h"
#include "Objects/TitleObjects/TitlePlayerAnimator.h"

#include "Objects/BossAnimation/BossAnimation.h"
#include "Objects/TitleObjects/TitleEarth.h"
#include "Objects/CameraState/CameraStateBossEntryToGame.h"
#include "Objects/Tornado/Tornado.h"
#include "Objects/BeatBossSprite/BeatBossSprite.h"


void BossEntryScene::Initialize() {

	Ring::ResetInstanceCount();
	Wind::ResetInstanceCount();


	/// main camera setting
	mainCamera_->GetTransform()->rotateOrder = XYZ;

	directionalLight_->SetDirection(Vec3(1.0f, 0.f, 1.0f).Normalize());

	/// objects instance create
	bossAnimation_ = new BossAnimation();
	cameraState_   = new CameraStateBossEntryToGame();
	TitleEarth* earth = new TitleEarth();
	TitlePlayerAnimator* playerAnimation = new TitlePlayerAnimator();
	BeatBossSprite* beatBossSpriteUp = new BeatBossSprite();
	BeatBossSprite* beatBossSpriteDown = new BeatBossSprite();
	GameCamera* spriteCamera = new GameCamera("spriteCamera");


	bossAnimation_->Initialize();
	cameraState_->Initialize();
	earth->Initialize();
	playerAnimation->Initialize();
	spriteCamera->Initialize();
	beatBossSpriteUp->Initialize();
	beatBossSpriteDown->Initialize();


	/// other setting
	bossAnimation_->SetPosition({ 0.0f, 11.0f, 0.0f });
	bossAnimation_->SetGameCamera(mainCamera_);
	bossAnimation_->SetAnimationIndex(BOSS_ANIMATION_ENTRY_CAMERA_MOVE);

	cameraState_->SetGameCamera(mainCamera_);
	cameraState_->isActive = false;

	playerAnimation->SetBasePosition({ 0.0f,0.0f, -12.0f });
	playerAnimation->SetRotateX(-std::numbers::pi_v<float> / 2.0f);
	playerAnimation->SetIsSpinUpdate(false);

	beatBossSpriteUp->SetRotateZ(std::numbers::pi_v<float>);
	beatBossSpriteUp->SetPositionY(0.875f);

	/// scene setting
	AddLayer("sprite", spriteCamera);
	spriteCamera->SetProjectionType(ORTHOGRAPHIC);

}

void BossEntryScene::Update() {

	const AnimationData& data = bossAnimation_->GetAnimatinData(BOSS_ANIMATION_ENTRY_TUBE_DOWN);

	/// 振り下ろしが終了したので遷移する
	if(data.time / data.maxTime >= 1.0f) {
		cameraState_->isActive = true;
		if(cameraState_->IsFinishedMoving()) {
			//SceneManager::GetInstance()->SetNextScene(GAME);
		}
	}

}
