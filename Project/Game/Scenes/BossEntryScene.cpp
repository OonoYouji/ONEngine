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

	Wind::ResetInstanceCount();


	/// main camera setting
	mainCamera_->GetTransform()->rotateOrder = XYZ;

	directionalLight_->SetDirection(Vec3(1.0f, 0.f, 1.0f).Normalize());

	/// objects instance create
	bossAnimation_ = new BossAnimation();
	cameraState_   = new CameraStateBossEntryToGame();
	TitleEarth*          earth           = new TitleEarth();
	TitlePlayerAnimator* playerAnimation = new TitlePlayerAnimator();
	beatBossSpriteArray_[0]              = new BeatBossSprite();
	beatBossSpriteArray_[1]              = new BeatBossSprite();
	GameCamera*          spriteCamera    = new GameCamera("spriteCamera");


	bossAnimation_->Initialize();
	cameraState_->Initialize();
	earth->Initialize();
	playerAnimation->Initialize();
	spriteCamera->Initialize();
	beatBossSpriteArray_[0]->Initialize();
	beatBossSpriteArray_[1]->Initialize();


	/// other setting
	bossAnimation_->SetPosition({ 0.0f, 11.0f, 0.0f });
	bossAnimation_->SetGameCamera(mainCamera_);
	bossAnimation_->SetAnimationIndex(BOSS_ANIMATION_ENTRY_CAMERA_MOVE);

	cameraState_->SetGameCamera(mainCamera_);
	cameraState_->isActive = false;

	playerAnimation->SetBasePosition({ 0.0f,0.0f, -12.0f });
	playerAnimation->SetRotateX(-std::numbers::pi_v<float> / 2.0f);
	playerAnimation->SetIsSpinUpdate(false);

	beatBossSpriteArray_[1]->SetRotateZ(std::numbers::pi_v<float>);
	beatBossSpriteArray_[1]->SetPositionY(0.55f);

	/// scene setting
	AddLayer("sprite", spriteCamera);
	spriteCamera->SetProjectionType(ORTHOGRAPHIC);
	spriteCamera->SetDistance(3.0f);


	bgm_.reset(new AudioSource);
	bgm_->Initialize();
	bgm_->SetAudioClip("bossEntryArart.wav");
	bgm_->isLoop = true;
	bgm_->volume = 0.1f;
	bgm_->PlayAudio();

}

void BossEntryScene::Update() {

	const AnimationData& data = bossAnimation_->GetAnimatinData(BOSS_ANIMATION_ENTRY_TUBE_DOWN);

	/// 振り下ろしが終了したので遷移する
	if(data.time / data.maxTime >= 1.0f) {
		cameraState_->isActive = true;
		for(size_t i = 0; i < 2; ++i) {
			beatBossSpriteArray_[i]->SetIsFadeOut(true);
		}

		if(cameraState_->IsFinishedMoving()) {
			SceneManager::GetInstance()->SetNextScene(GAME);
			bgm_->StopAudioAll();
		}
	}

}
