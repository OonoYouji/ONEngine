#include "Scene_Title.h"

/// engine
#include "Core/ONEngine.h"
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"

#include "Objects/TitleObjects/TitleTextRenderer/TitleTextRenderer.h"
#include "Objects/TitleObjects/TitleMenuSelector/TitleMenuSelector.h"
#include "Objects/SceneTransition/SceneTransition.h"
	

void Scene_Title::Initialize() {

	/// create objects  -----------------------------------------------------
	(new TitleTextRenderer())->Initialize();
	menuSelector_ = new TitleMenuSelector();
	menuSelector_->Initialize();
	

	/// add layer -----------------------------------------------------

	GameCamera* uiCamera = new GameCamera("uiCamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);

	GameCamera* sceneTransitionCamera = new GameCamera("sceneTransitionCamera");
	sceneTransitionCamera->Initialize();
	sceneTransitionCamera->SetProjectionType(ORTHOGRAPHIC);


	AddLayer("uiLayer", uiCamera);
	AddLayer("sceneTransitionLayer", sceneTransitionCamera);


	sceneTransition_ = nullptr;
}

void Scene_Title::Update() {

	/// ゲームを開始した
	if(menuSelector_->GetIsSelected()) {

		if(!sceneTransition_) {
			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 1.0f, TITLE_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		}
	}



	if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
		SceneManager* sceneManager = SceneManager::GetInstance();

		switch(menuSelector_->GetSelectMenu()) {
		case SELECT_MENU_START:
			sceneManager->SetNextScene(GAME);
			break;
		case SELECT_MENU_EXIT:
			ONEngine::SetIsRunning(false);
			break;
		}
	}
}
