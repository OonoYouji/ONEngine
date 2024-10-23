#include "Scene_Title.h"

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/Tornado/Tornado.h"
#include "Objects/TitleObjects/TitleEarth.h"
#include "Objects/TitleObjects/TitlePlayerAnimator.h"
#include "Objects/TitleObjects/GameTilteRendere.h"
#include "Objects/SceneTransition/SceneTransition.h"
#include "Objects/SpaceSpriteRenderer/SpaceSpriteRenderer.h"


void Scene_Title::Initialize() {

	/// reset...
	Wind::ResetInstanceCount();
	
	/// camera setting
	Transform* cameraTransform   = mainCamera_->GetTransform();
	cameraTransform->position    = { 2.914f, 15.186f, -11.229f};
	cameraTransform->rotate      = { 0.238f, -0.445f, 0.0f };
	cameraTransform->rotateOrder = XYZ;

	/// intance create
	GameCamera*          uiCamera          = new GameCamera("UICamera");
	TitleEarth*          earth             = new TitleEarth();
	TitlePlayerAnimator* playerAnimator    = new TitlePlayerAnimator();
	GameTitleRenderer*   gameTitleRenderer = new GameTitleRenderer();
	sceneTransition_ = new SceneTransition(TRANSTION_FADE_IN);
	SpaceSpriteRenderer* spaceSpriteRenderer = new SpaceSpriteRenderer();

	/// initializing
	uiCamera->Initialize();
	earth->Initialize();
	playerAnimator->Initialize();
	gameTitleRenderer->Initialize();
	sceneTransition_->Initialize();
	spaceSpriteRenderer->Initialize();

	/// setting...
	uiCamera->SetPositionZ(-10.0f);
	AddLayer("ui", uiCamera);
	AddLayer("scene transition", uiCamera);

	playerAnimator->SetParticleUseRotate(false);
	playerAnimator->GetParticleSystem()->SetEmittedParticleCount(4u);

	/// 
	audioSource_.reset(new AudioSource);
	audioSource_->Initialize();
	audioSource_->SetAudioClip("gameStart.wav");


	/// draw layer setting
	sceneTransition_->drawLayerId = 2;
	spaceSpriteRenderer->drawLayerId = 1;


	/// game title sprite
	gameTitleRenderer->SetPosition({ -1.4f, 0.9f, 0.0f });
	gameTitleRenderer->SetScale({ 4.8f, 3.0f, 0.0f });

	/// push space sprite
	spaceSpriteRenderer->SetPosition({ -1.5f, -0.7f, 0.0f });
	spaceSpriteRenderer->SetScale({ 3.1f, 1.7f, 0.0f });

}

void Scene_Title::Update() {

	if(Input::TriggerKey(KeyCode::Space) || Input::TriggerPadButton(PadCode::A)) {
		sceneTransition_->SetIsStarted(true);
	}

	if(sceneTransition_->GetIsFinished()) {
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::TUTORIAL);
		audioSource_->PlayAudio();
	}
}
