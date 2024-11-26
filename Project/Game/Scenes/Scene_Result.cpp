#include "Scene_Result.h"


/// engine
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"

/// objects
#include "Objects/Camera/GameCamera.h"
#include "Objects/SceneTransition/SceneTransition.h"

#include "Objects/TitleObjects/TitleMenuSelector/TitleMenuSelector.h"


void Scene_Result::Initialize() {


	/// 仮オブジェクト
	(new TitleMenuSelector)->Initialize();

	
	
	
	/// add layer ---------------------------------------------------------------------
	
	GameCamera* uiCamera = new GameCamera("UICamera");
	uiCamera->Initialize();
	uiCamera->SetProjectionType(ORTHOGRAPHIC);

	AddLayer("UILayer", uiCamera);



	GameCamera* transitionCamera = new GameCamera("TransitionCamera");
	transitionCamera->Initialize();
	transitionCamera->SetProjectionType(ORTHOGRAPHIC);
	AddLayer("TransitionLayer", transitionCamera);

}

void Scene_Result::Update() {


	/// ゲームを開始した
	//if(menuSelector_->GetIsSelected()) {
	bool isTransitio = false;
	isTransitio |= Input::TriggerKey(KeyCode::Space);
	isTransitio |= Input::TriggerPadButton(PadCode::A);

	if(isTransitio) {

		if(!sceneTransition_) {
			sceneTransition_ = new SceneTransition(TRANSITION_TYPE_IN, 1.0f, RESULT_LAYER_TRANSITION);
			sceneTransition_->Initialize();
		}
	}



	if(sceneTransition_ && sceneTransition_->GetIsEnd()) {
		SceneManager* sceneManager = SceneManager::GetInstance();
		sceneManager->SetNextScene(TITLE);

		//switch(menuSelector_->GetSelectMenu()) {
		//case SELECT_MENU_START:
		//	sceneManager->SetNextScene(GAME);
		//	break;
		//case SELECT_MENU_EXIT:
		//	ONEngine::SetIsRunning(false);
		//	break;
		//}
	}

}
