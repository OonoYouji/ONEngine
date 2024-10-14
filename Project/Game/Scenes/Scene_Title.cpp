#include "Scene_Title.h"

/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/TitleObjects/TitleEarth.h"
#include "Objects/TitleObjects/TitlePlayerAnimator.h"

void Scene_Title::Initialize() {

	/// intance create
	TitleEarth* earth                   = new TitleEarth();
	TitlePlayerAnimator* playerAnimator = new TitlePlayerAnimator();

	/// initializing
	earth->Initialize();
	playerAnimator->Initialize();

	/// setting...


	/// camera setting
	mainCamera_->SetPosition({ 0.274f, 1.245f, -1.237f});
	mainCamera_->SetRotate({ 0.125f, -0.398f, 0.0f });
	mainCamera_->isActive = false;
	mainCamera_->UpdateMatrix();

}

void Scene_Title::Update() {

	if(Input::TriggerKey(KeyCode::Space)) {
		SceneManager::GetInstance()->SetNextScene(SCENE_ID::GAME);
	}

}
