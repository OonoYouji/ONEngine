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



void Scene_Title::Initialize() {

	/// reset...
	Ring::ResetInstanceCount();
	
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

	/// initializing
	uiCamera->Initialize();
	earth->Initialize();
	playerAnimator->Initialize();
	gameTitleRenderer->Initialize();

	/// setting...
	uiCamera->SetPositionZ(-10.0f);
	AddLayer("ui", uiCamera);

}

void Scene_Title::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::BOSS_ENTRY);
	}

}
