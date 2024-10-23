#include "Scene_Result.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/GameResult/GameResult.h"

#include "Objects/Tornado/Tornado.h"
#include "Objects/DefeatStamp/DefeatStamp.h"


void Scene_Result::Initialize() {

	/// reset
	Wind::ResetInstanceCount();


	/// instance create
	GameCamera* spriteCamera = new GameCamera("spriteCamera");
	gameResult_ = new GameResult();
	DefeatStamp* defeatStamp = new DefeatStamp();

	/// initializing
	spriteCamera->Initialize();
	gameResult_->Initialize();
	defeatStamp->Initialize();

	/// other setting
	spriteCamera->SetProjectionType(ORTHOGRAPHIC);
	spriteCamera->SetDistance(6.0f);
	AddLayer("sprite", spriteCamera);
	AddLayer("transition", spriteCamera);


	mainCamera_->GetTransform()->rotateOrder = XYZ;
	mainCamera_->SetPosition({ -10.0f, 5.5f, -15.725f });
	mainCamera_->SetRotate({ 0.308f, 0.8f, 0.0f });


	bgm_.reset(new AudioSource);
	bgm_->Initialize();
	bgm_->SetAudioClip("resultBgm.wav");
	bgm_->isLoop = true;
	bgm_->volume = 0.1f;
	bgm_->PlayAudio();

}

void Scene_Result::Update() {

	if(gameResult_->IsNextSceneTransitioned()) {
		bgm_->StopAudioAll();
	}

}
